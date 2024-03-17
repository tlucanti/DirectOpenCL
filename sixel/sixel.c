
#include <sixel.h>

#include <termios.h>

#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

typedef unsigned int col_t;

struct canvas {
	int width;
	int height;
	col_t *buf;
};

static int sixel_write(char *data, int size, void *out)
{
	return fwrite(data, 1, size, (FILE *)out);
}

void set_pixel(struct canvas *canvas, int x, int y, col_t color)
{
	canvas->buf[canvas->width * y + x] = color;
}

#define square(x) ((x) * (x))

void draw_circle(struct canvas *canvas, int x, int y, int radius, col_t color)
{
	for (int yy = y - radius / 2; yy < y + radius / 2; yy++) {
		for (int xx = x - radius / 2; xx < x + radius / 2; xx++) {
			if (square(xx - x) + square(yy - y) <= radius) {
				set_pixel(canvas, xx, yy, color);
			}
		}
	}
}

void draw_borders(struct canvas *canvas, col_t color)
{
	for (int x = 0; x < canvas->width; x++) {
		for (int y = 0; y < canvas->height; y++) {
			if (x == 0 || x == canvas->width - 1 ||
				y == 0 || y == canvas->height - 1) {
				set_pixel(canvas, x, y, color);
			}
		}
	}
}

static float get_fps(void)
{
	static struct timespec ts = {};
	time_t prev_s = ts.tv_sec;
	long prev_ns = ts.tv_nsec;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return 1.f / ((ts.tv_sec - prev_s) + (ts.tv_nsec - prev_ns) * 1e-9f);
}

void draw()
{
	sixel_output_t *output;
	sixel_dither_t *dither;
	struct canvas canvas;
	SIXELSTATUS status;

	canvas.width = 1200;
	canvas.height = 200;
	canvas.buf = malloc(sizeof(int) * canvas.width * canvas.height);
	if (canvas.buf == NULL) {
		printf("canvas alloc fail\n");
		return;
	}

	status = sixel_output_new(&output, sixel_write, stdout, NULL);
	if (SIXEL_FAILED(status)) {
		printf("sixel_output_new() fail\n");
		return;
	}

	status = sixel_dither_new(&dither, SIXEL_PALETTE_MAX, NULL);
	if (SIXEL_FAILED(status)) {
		printf("sixel_dither_new() failed\n");
	}

	sixel_output_set_encode_policy(output, SIXEL_ENCODEPOLICY_FAST);

	int y = 50;
	int col = 0;

next:
	for (int x = 0; x < 256; x += 4) {
		draw_circle(&canvas, x * 4, y, 40, x << col);
		draw_borders(&canvas, x << col);

		printf("\e[0;0H");
		fflush(stdout);

		sixel_dither_initialize(dither, (void *)canvas.buf, canvas.width, canvas.height,
					SIXEL_PIXELFORMAT_RGBA8888,
					SIXEL_LARGE_AUTO,
					SIXEL_REP_AUTO,
					SIXEL_QUALITY_FULL);
		status = sixel_encode((void *)canvas.buf, canvas.width, canvas.height, 0, dither, output);
		if (SIXEL_FAILED(status)) {
			printf("sixel_encode() fail\n");
			return;
		}

		printf("fps: %f\n", get_fps());
		//usleep(10000);
	}
	if (col == 0) {
		col = 8;
		y = 100;
		goto next;
	} else if (col == 8) {
		col = 16;
		y = 150;
		goto next;
	}
}


struct termios term_info;

void finalize(int sig)
{
	(void)sig;
	int err;

	//err = tcsetattr(STDIN_FILENO, TCSANOW, &term_info);
	//if (err != 0) {
	//	perror("tcsetattr");
	//	abort();
	//}

	printf("\e[?1000l");
	fflush(stdout);
	printf("\ndone\n");
	exit(0);
}

void tty_raw()
{
	struct termios raw_mode;
	int err;
	err = 0;

	// Get the current terminal characteristics.
	err = tcgetattr(STDIN_FILENO, &term_info);
	if (err != 0) {
		perror("tcgetattr");
		finalize(0);
	}

	// Copy what we got, so we can change it.
	memcpy(&raw_mode, &term_info, sizeof(struct termios));

	// Modify the terminal characteristics to 'raw mode'.
	cfmakeraw(&raw_mode);

	// Set the terminal to raw mode.
	err = tcsetattr(STDIN_FILENO, TCSANOW, &raw_mode);
	if (err != 0) {
		perror("tcsetattr");
	finalize(0);
	}
}

void key_reader(void)
{
	while (true) {
		int button, x, y;
		char c, type;
		int success;

		success = scanf("%c", &c);
		if (success == 0) {
			printf("cannot read from stdin\n");
			finalize(0);
		}

		if (c == '\n') {
		} else if (c == '\e') {
			success = scanf("[<%d;%d;%d%c", &button, &x, &y, &type);
			if (success == 4) {
				if (button == 35) {
					printf("mouse move to");
				} else if (button == 65) {
					printf("scroll down at");
				} else if (button == 64) {
					printf("scroll up at");
				} else if (button <= 7 && type == 'M') {
					printf("button %d pressed", button);
				} else if (button <= 7 && type == 'm') {
					printf("button %d released at", button);
				} else {
					printf("UNKNOWN EVENT %d %c", button, type);
				}
				printf(" x=%d y=%d\n", x, y);
			}
		} else if (c == 'x') {
			finalize(0);
		} else {
			printf("pressed %d (%c)\n", c, c);
		}
	}
}

int main()
{
	signal(SIGINT, finalize);
	signal(SIGTERM, finalize);

	printf("\e[?1003h\e[?1015h\e[?1006h");
	//tty_raw();
	fflush(stdout);

	key_reader();
	return 0;

	while (true) {
		char c;

		read(STDIN_FILENO, &c, 1);
		printf("got %d", c);
		if (isprint(c)) {
			printf(" (%c)", c);
		}
		printf("\r\n");
		fflush(stdout);

		if (c == 'x') {
			finalize(0);
		}
		//printf("%d\n", c);
	}
}
