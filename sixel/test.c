#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>


typedef int fd_t;

const fd_t STDIN  = STDIN_FILENO;
const fd_t STDOUT = STDIN_FILENO;
const fd_t STDERR = STDIN_FILENO;

static void assert_tty(fd_t fd);
static void set_terminal(fd_t fd, struct termios *term_mode);

void show_chars()
{
    int last_char;             /* Last char read  */
    size_t left;              /* Number for chars read in last read */
    char buff[256] = {};      /* Zero the buffer */
    char *curr;

    last_char = '\0';
    left = 0;

    while (last_char != 'x') {
        if (left > 0) {
            last_char = *curr;
            curr++;
            left--;
            printf("Got char %3d\r\n", (int) last_char);
        } else {
            left = read(STDIN, buff, sizeof(buff));
            if (left < 1) {
                perror("read");
                return;
            }
            curr = &buff[0];    /* Redundant & and [], but it makes
                                 * sense */
        }
    }
}

int main(int argc, char *argv[])
{
    struct termios term_info;
    struct termios raw_mode;
    int err;
    err = 0;

    // Ensure we're working with a tty
    assert_tty(STDIN);

    // Get the current terminal characteristics.
    err = tcgetattr(STDIN, &term_info);
    if (err != 0) {
        perror("tcgetattr");
        exit(1);
    }
    // Copy what we got, so we can change it.
    memcpy(&raw_mode, &term_info, sizeof(struct termios));
    // Modify the terminal characteristics to 'raw mode'.
    cfmakeraw(&raw_mode);
    // Set the terminal to raw mode.
    set_terminal(STDIN, &raw_mode);
    // Run the loop
    show_chars();
    // Reset the terminal
    set_terminal(STDIN, &term_info);

    printf("Done!\n");
    return EXIT_SUCCESS;
}

static void set_terminal(fd_t fd, struct termios *term_mode)
{
    int err;
    err = tcsetattr(STDIN, TCSANOW, term_mode);
    if (err != 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

static void assert_tty(fd_t fd)
{
    if (!isatty(fd)) {
        fprintf(stderr, "fd(%d) is not a tty\n", fd);
        exit(EXIT_FAILURE);
    }
}
