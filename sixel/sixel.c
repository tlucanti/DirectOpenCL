
#include <guilib.h>
#include <sixel.h>

#include <pthread.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ESCAPE_CLEAR_SCREEN "\033[0;0H"
#define ESCAPE_HIDE_CURSOR "\033[?25l"
#define ESCAPE_SHOW_CURSOR "\033[?25h"
#define ESCAPE_ENABLE_LOCATOR "\033[?1003h" "\033[?1015h" "\033[?1006h" "\033[?1016h"
#define ESCAPE_DISABLE_LOCATOR "\e[?1000l"

#if CONFIG_GUILIB_SIXEL_WARN_UNKWONN_ESCAPE
# define WARN_UNKNOWN_ESCAPE(c) fprintf_lock(stderr, "unknow escape sequence: %d (%c)", (c), isprint(c) ? (c) : ' ');
#else
# define WARN_UNKNOWN_ESCAPE(c) /* empty */
#endif

#define MAX_FPS CONFIG_GUILIB_SIXEL_MAX_FPS_RELAX
#define DELAY CONFIG_GUILIB_SIXEL_KEYBOARD_DELAY

#define fprintf_lock(...)                           \
	do {                                       \
		pthread_spin_lock(&stdout_lock);   \
		fprintf(__VA_ARGS__);               \
		pthread_spin_unlock(&stdout_lock); \
	} while (false)

#define gui_perror_lock(...)                       \
	do {                                       \
		pthread_spin_lock(&stdout_lock);   \
		gui_perror(__VA_ARGS__);           \
		pthread_spin_unlock(&stdout_lock); \
	} while (false)

struct gui_window {
	unsigned int width;
	unsigned int height;
	unsigned int length;
	key_hook_t callback;
	unsigned int *raw_pixels;
	sixel_output_t *sixel_output;
	sixel_dither_t *sixel_dither;
};

struct key_metadata {
	double last_press_time;
	int keycode;
	bool released;
};

static struct termios g_term_info;

static struct gui_window *g_window = NULL;
static unsigned char g_pressed_keys[256];
static int g_mouse_x = 0;
static int g_mouse_y = 0;
static double g_last_interrupt_time = 0;
static pthread_t g_key_reader_thread_id = 0;
static bool g_key_reader_thread_should_stop = false;

static pthread_spinlock_t stdout_lock;
static pthread_spinlock_t key_lock;

unsigned char key_table[256] = {
	['0'] = KEY_0,		[')'] = KEY_0,
	['1'] = KEY_1,		['!'] = KEY_1,
	['2'] = KEY_2,		['@'] = KEY_2,
	['3'] = KEY_3,		['#'] = KEY_3,
	['4'] = KEY_4,		['$'] = KEY_4,
	['5'] = KEY_5,		['%'] = KEY_5,
	['6'] = KEY_6,		['^'] = KEY_6,
	['7'] = KEY_7,		['&'] = KEY_7,
	['8'] = KEY_8,		['*'] = KEY_8,
	['9'] = KEY_9,		['('] = KEY_9,

	['a'] = KEY_A,		['A'] = KEY_A,		/* ['\x01'] = KEY_A, */
	['b'] = KEY_B,		['B'] = KEY_B,		/* ['\x02'] = KEY_B, */
	['c'] = KEY_C,		['C'] = KEY_C,		/* ['\x03'] = KEY_C, */
	['d'] = KEY_D,		['D'] = KEY_D,		/* ['\x04'] = KEY_D, */
	['e'] = KEY_E,		['E'] = KEY_E,		/* ['\x05'] = KEY_E, */
	['f'] = KEY_F,		['F'] = KEY_F,		/* ['\x06'] = KEY_F, */
	['g'] = KEY_G,		['G'] = KEY_G,		/* ['\x07'] = KEY_G, */
	['h'] = KEY_H,		['H'] = KEY_H,		/* ['\x08'] = KEY_H, */
	['i'] = KEY_I,		['I'] = KEY_I,		/* ['\x09'] = KEY_I, */
	['j'] = KEY_J,		['J'] = KEY_J,		/* ['\x0A'] = KEY_J, */
	['k'] = KEY_K,		['K'] = KEY_K,		/* ['\x0B'] = KEY_K, */
	['l'] = KEY_L,		['L'] = KEY_L,		/* ['\x0C'] = KEY_L, */
	['m'] = KEY_M,		['M'] = KEY_M,		/* ['\x0D'] = KEY_M, */
	['n'] = KEY_N,		['N'] = KEY_N,		/* ['\x0E'] = KEY_N, */
	['o'] = KEY_O,		['O'] = KEY_O,		/* ['\x0F'] = KEY_O, */
	['p'] = KEY_P,		['P'] = KEY_P,		/* ['\x10'] = KEY_P, */
	['q'] = KEY_Q,		['Q'] = KEY_Q,		/* ['\x11'] = KEY_Q, */
	['r'] = KEY_R,		['R'] = KEY_R,		/* ['\x12'] = KEY_R, */
	['s'] = KEY_S,		['S'] = KEY_S,		/* ['\x13'] = KEY_S, */
	['t'] = KEY_T,		['T'] = KEY_T,		/* ['\x14'] = KEY_T, */
	['u'] = KEY_U,		['U'] = KEY_U,		/* ['\x15'] = KEY_U, */
	['v'] = KEY_V,		['V'] = KEY_V,		/* ['\x16'] = KEY_V, */
	['w'] = KEY_W,		['W'] = KEY_W,		/* ['\x16'] = KEY_W, */
	['x'] = KEY_X,		['X'] = KEY_X,		/* ['\x17'] = KEY_X, */
	['y'] = KEY_Y,		['Y'] = KEY_Y,		/* ['\x18'] = KEY_Y, */
	['z'] = KEY_Z,		['Z'] = KEY_Z,		/* ['\x19'] = KEY_Z, */

	// []  = KEY_ESCAPE,
	[126]  = KEY_DELETE,
	['`']  = KEY_BACKQUOTE,		/* ['~'] = KEY_BACKQUOTE, */
	['-']  = KEY_MINUS,		['_'] = KEY_MINUS,
	['=']  = KEY_EQUAL,		['+'] = KEY_EQUAL,
	[127]  = KEY_BACKSPACE,
	['[']  = KEY_OPEN_BRACKET,	['{'] = KEY_OPEN_BRACKET,
	[']']  = KEY_CLOSE_BRACKET,	['}'] = KEY_CLOSE_BRACKET,
	['\\'] = KEY_BACKSLASH,		['|'] = KEY_BACKSLASH,
	[';']  = KEY_SEMICOLON,		[':'] = KEY_SEMICOLON,
	['\''] = KEY_QUOTE,		['"'] = KEY_QUOTE,
	['\n'] = KEY_ENTER,		[13]  = KEY_ENTER,
	[',']  = KEY_COMA,		['<'] = KEY_COMA,
	['.']  = KEY_POINT,		['>'] = KEY_POINT,
	['/']  = KEY_SLASH,		['?'] = KEY_SLASH,
	[' ']  = KEY_SPACE,

	['\t'] = KEY_TAB,
	// [] = KEY_CAPS,
	// [] = KEY_LSHIFT,
	// [] = KEY_LCTRL,
	// [] = KEY_LWIN,
	// [] = KEY_LALT,
	// [] = KEY_RSHIFT,
	// [] = KEY_RCTRL,
	// [] = KEY_RWIN,
	// [] = KEY_RALT,

	[141] = KEY_F1,
	[142] = KEY_F2,
	[143] = KEY_F3,
	[144] = KEY_F4,
	[145] = KEY_F5,
	[146] = KEY_F6,
	[147] = KEY_F7,
	[148] = KEY_F8,
	[149] = KEY_F9,
	[150] = KEY_F10,
	// [151] = KEY_F11,
	[152] = KEY_F12,
	// [153] = KEY_F13,
	// [154] = KEY_F14,
	// [155] = KEY_F15,
	// [156] = KEY_F16,
	// [157] = KEY_F17,
	// [158] = KEY_F18,
	// [159] = KEY_F19,

	[160] = KEY_NUM_0,
	[161] = KEY_NUM_1,
	[162] = KEY_NUM_2,
	[163] = KEY_NUM_3,
	[164] = KEY_NUM_4,
	[165] = KEY_NUM_5,
	[166] = KEY_NUM_6,
	[167] = KEY_NUM_7,
	[168] = KEY_NUM_8,
	[169] = KEY_NUM_9,
};

__noret
static void gui_abort(void)
{
	WRITE_ONCE(g_key_reader_thread_should_stop, true);
	if (g_key_reader_thread_id != 0) {
		pthread_join(g_key_reader_thread_id, NULL);
	}

	if (g_window != NULL) {
		gui_destroy(g_window);
	}

	gui_finalize();
	exit(1);
	unreachable();
}

static void sighandler(int signum)
{
	switch (signum) {
	case SIGINT:
		gui_perror_lock("got SIGINT: aborting");
		break;
	case SIGABRT:
		gui_perror_lock("got SIGABRT: aborting");
		break;
	case SIGSEGV:
		gui_perror_lock("got SIGSEGV: aborting");
		break;
	}

	gui_abort();
}

int do_getc(void)
{
	int c = getc(stdin);
	// fprintf_lock(stderr, "\tread (%d) %c\n\r", (int)c, isprint(c) ? c : ' ');
	return c;
}

static void fps_relax(void)
{
	static struct timespec ts = {};
	time_t prev_s = ts.tv_sec;
	long prev_ns = ts.tv_nsec;
	float fps;

	do {
		clock_gettime(CLOCK_MONOTONIC, &ts);
		fps = 1.f / ((ts.tv_sec - prev_s) + (ts.tv_nsec - prev_ns) * 1e-9f);
		usleep(5000);
	} while (fps > MAX_FPS);
}

static double time_now(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void do_key_callback(int keycode, bool pressed)
{
	WRITE_ONCE(g_pressed_keys[keycode], pressed);
	WRITE_ONCE(g_last_interrupt_time, time_now());
	if (g_window->callback == NULL) {
		return;
	}
	g_window->callback(g_window, keycode, pressed);
}

static void string_write(int fd, const char *s)
{
	if (write(fd, s, strlen(s)) < 0) {
                gui_panic("string write error");
        }
}

static void locator_enable(bool enable)
{
	if (!CONFIG_GUILIB_SIXEL_MOUSE_ENABLE) {
		return;
	}

	if (enable) {
		string_write(STDOUT_FILENO, ESCAPE_ENABLE_LOCATOR);
	} else {
		string_write(STDOUT_FILENO, ESCAPE_DISABLE_LOCATOR);
	}
}

static int tty_init(void)
{
	if (tcgetattr(STDIN_FILENO, &g_term_info) != 0) {
		gui_perror("tcsetattr fail");
		return EINVAL;
	}
	return 0;
}

static int tty_raw(bool enable)
{
	struct termios raw_mode;

	if (!enable) {
		if (tcsetattr(STDIN_FILENO, TCSANOW, &g_term_info) != 0) {
			gui_perror("tcsetattr fail");
			return EINVAL;
		}
		return 0;
	}

	raw_mode = g_term_info;
	cfmakeraw(&raw_mode);

	if (tcsetattr(STDIN_FILENO, TCSANOW, &raw_mode) != 0) {
		gui_perror("tcsetattr fail");
		tty_raw(false);
		return EINVAL;
	}

	return 0;
}

static int sixel_write(char *data, int size, void *arg)
{
	(void)arg;
	if (CONFIG_GUILIB_SIXEL_NO_DRAW) {
		printf("writing sixel data of size %d\r\n", size);
		return 0;
	}

	if (write(STDOUT_FILENO, data, size) < 0) {
                gui_panic("stdout write fail");
        }
	return 0;
}

static void *release_checker_thread(void *arg)
{
	struct key_metadata *meta = arg;

	usleep(DELAY * 1e6f);
	pthread_spin_lock(&key_lock);
	if (time_now() - meta->last_press_time > DELAY && !meta->released) {
		meta->released = true;
		pthread_spin_unlock(&key_lock);
		do_key_callback(meta->keycode, false);
	} else {
		pthread_spin_unlock(&key_lock);
	}
	return NULL;
}

static void key_reader_event_tracker(int keycode)
{
	static struct key_metadata meta[256] = {};
	static bool init = false;
	pthread_t tid;
	double time;
	bool do_callback;

	if (keycode == 0) {
		return;
	}

	if (!init) {
		init = true;
		for (int i = 0; i < 256; i++) {
			meta[i].released = false;
			meta[i].last_press_time = 0;
			meta[i].keycode = i;
		}
	}

	if (keycode >= 256) {
                gui_perror_lock("keycode overflow");
                return;
	}

	time = time_now();
	do_callback = (time - meta[keycode].last_press_time > DELAY);
	meta[keycode].last_press_time = time;
	if (pthread_create(&tid, NULL, release_checker_thread, &meta[keycode])) {
		gui_perror_lock("release_checker_thread spawn fail");
		abort();
	}
	if (pthread_detach(tid)) {
		gui_perror_lock("release_checker_thread detach fail");
		abort();
	}

	if (do_callback) {
		pthread_spin_lock(&key_lock);
		meta[keycode].released = false;
		pthread_spin_unlock(&key_lock);
		do_key_callback(keycode, true);
	}
}

static void *key_reader_thread(void *arg)
{
	int button, x, y;
	int c;
	unsigned char type;
	int success;
	bool pressed;

	(void)arg;

	while (!READ_ONCE(g_key_reader_thread_should_stop)) {
		c = do_getc();

		switch (c) {
		case EOF:
			/* read fail */
			goto read_fail;
		// case 'x':
		// 	/* exit if 'x' pressed */
		// 	do_key_callback(GUI_CLOSED, true);
		// 	return NULL;
		case '\n':
			/* if raw mode disabled '\n' should be ignored */
			if (!CONFIG_GUILIB_SIXEL_RAW_MODE) {
				continue;
			}
		case '\e':
			/* escape sequence, going to decode */
			break;
		default:
			/* just regular letter or digit */
			key_reader_event_tracker(key_table[c]);
			continue;
		}
		// continue;

		/**
		 * now we are decoding escape sequence in state:
		 * \e
		 */
		c = do_getc();
		switch (c) {
		case EOF:
			/* read fail */
			goto read_fail;
		case '[':
			/* next escape sequence state */
			break;
		case 'O':
			/* escape sequence for (F1, F2, F3, F4) keys */
			c = do_getc();

			switch (c) {
			case EOF:
				goto read_fail;
			case 'P':
				/* KEY_F1 key sequence is:
				 * \eOP
				 */
				key_reader_event_tracker(KEY_F1);
				break;
			case 'Q':
				/* KEY_F2 key sequence is:
				 * \eOQ
				 */
				key_reader_event_tracker(KEY_F2);
				break;
			case 'R':
				/* KEY_F3 key sequence is:
				 * \eOR
				 */
				key_reader_event_tracker(KEY_F3);
				break;
			case 'S':
				/* KEY_F4 key sequence is:
				 * \eOS
				 */
				key_reader_event_tracker(KEY_F4);
				break;
			default:
				WARN_UNKNOWN_ESCAPE(c);
			}
			continue;
		default:
			WARN_UNKNOWN_ESCAPE(c);
			continue;
		}

		/* now we are decoding escape sequence in state:
		 * \e[
		 */
		c = do_getc();
		switch (c) {
		case EOF:
			/* read fail */
			goto read_fail;
		case 'A':
			/* KEY_LEFT key sequence is:
			 * \e[A
			 */
			key_reader_event_tracker(KEY_UP);
			break;
		case 'B':
			/* KEY_DOWN key sequence is:
			 * \e[B
			 */
			key_reader_event_tracker(KEY_DOWN);
			break;
		case 'C':
			/* KEY_RIGHT key sequence is:
			 * \e[C
			 */
			key_reader_event_tracker(KEY_RIGHT);
			break;
		case 'D':
			/* KEY_LEFT key sequence is:
			 * \e[D
			 */
			key_reader_event_tracker(KEY_LEFT);
			break;
		case '1':
		case '2':
			/* escape sequence for (F5, F6, F7, F8, F9, F10) keys */
			switch (c) {
			case '1':
				c = do_getc();
				switch (c) {
				case EOF:
					/* read fail */
					goto read_fail;
				case '5':
					button = KEY_F5;
					break;
				case '7': /* this is not a mistake, '6' is missed */
					button = KEY_F6;
					break;
				case '8':
					button = KEY_F7;
					break;
				case '9':
					button = KEY_F8;
					break;
				default:
					WARN_UNKNOWN_ESCAPE(c);
					continue;
				}
				break;
			case '2':
				c = do_getc();
				switch (c) {
				case EOF:
					/* read fail */
				case '0':
					button = KEY_F9;
					break;
				case '1':
					button = KEY_F10;
					break;
				/* KEY_F11 cannot be recorded */
				case '4':
					button = KEY_F12;
					break;
				}
				break;
			default:
				WARN_UNKNOWN_ESCAPE(c);
				continue;
			}

			c = do_getc();
			switch (c) {
			case EOF:
				/* read fail */
				goto read_fail;
			case '~':
				/* at the end of escape sequence ~ placed */
				break;
			default:
				WARN_UNKNOWN_ESCAPE(c);
				continue;
			}

			key_reader_event_tracker(button);
			break;

		case '3':
			/* escape sequence for DEL key */
			c = do_getc();

			switch (c) {
			case EOF:
				/* read fail */
				goto read_fail;
			case '~':
				/**
				 * KEY_DEL key sequence is as follows:
				 * \e[3~
				 * DEL is used for exiting
				 */
                		fprintf_lock(stderr, "prerssed 'DEL' exiting now\r\n");
				do_key_callback(GUI_CLOSED, true);
				return NULL;
			default:
				WARN_UNKNOWN_ESCAPE(c);
				continue;
			}
			break;

		case '<':
			/* escape sequence state for mouse events.
			 * now we are decoding escape sequence in state:
			 * \e[<
			 */
			success = scanf("%d;%d;%d%c", &button, &x, &y, &type);
			if (success == EOF) {
				goto read_fail;
			} else if (success != 4) {
				WARN_UNKNOWN_ESCAPE(c);
				continue;
			}

			switch (button) {
			case 0: /* MOUSE_LEFT */
			case 1: /* MOUSE_RIGHT */
			case 2: /* MOUSE_MIDDLE */
			case 3: /* MOUSE_3 */
			case 4: /* MOUSE_4 */
			case 5: /* MOUSE_5 */
			case 6: /* MOUSE_6 */
			case 7: /* MOUSE_7 */
				/**
				 * mouse button sequence are:
				 * \e[<{key};{x};{y};{type}
				 * where:
				 *   {key}: mouse key
				 *   {x}: x coord
				 *   {y}: y coord
				 *   {type}: type of event
				 */
				switch (type) {
				case 'M':
					/* mouse button pressed */
					pressed = true;
					break;
				case 'm':
					/* mouse button released */
					pressed = false;
					break;
				default:
					WARN_UNKNOWN_ESCAPE(c);
					continue;
				}

				do_key_callback(button, pressed);
				break;
			case 35:
				/* mouse move */
				break;
			case 64:
				/* mouse scroll up */
				do_key_callback(SCROLL_UP, true);
				break;
			case 65:
				/* mouse scroll down */
				do_key_callback(SCROLL_DOWN, true);
				break;
			default:
				WARN_UNKNOWN_ESCAPE(c);
			}
			WRITE_ONCE(g_mouse_x, x);
			WRITE_ONCE(g_mouse_y, y);
			WRITE_ONCE(g_last_interrupt_time, time_now());
		}
	}

	return NULL;

read_fail:
	pthread_spin_lock(&stdout_lock);
	gui_panic("stdin read error");
}

int gui_bootstrap(void)
{
	if (signal(SIGINT, sighandler) ||
	    signal(SIGABRT, sighandler) ||
	    signal(SIGSEGV, sighandler)) {
		gui_perror("set signal handler fail");
	}

	if (pthread_spin_init(&stdout_lock, false)) {
                gui_perror("stdout_lock init fail");
		goto fail;
	}
	if (pthread_spin_init(&key_lock, false)) {
		gui_perror("key_lock init fail");
		goto fail;
	}

	if (CONFIG_GUILIB_SIXEL_KEYBOARD_ENABLE) {
		if (pthread_create(&g_key_reader_thread_id, NULL, key_reader_thread, NULL)) {
			gui_perror("key_reader_thread spawn fail");
			goto fail;
		}
		locator_enable(true);
	}

	if (CONFIG_GUILIB_SIXEL_RAW_MODE) {
		if (tty_init()) {
                        gui_perror("tty init fail");
			goto fail;
		}
		if (tty_raw(true)) {
                        gui_perror("tty enable raw mode fail");
			goto fail;
		}
	}

	pthread_spin_lock(&stdout_lock);
	string_write(STDOUT_FILENO, ESCAPE_HIDE_CURSOR);
	pthread_spin_unlock(&stdout_lock);

	return 0;

fail:
	WRITE_ONCE(g_key_reader_thread_should_stop, true);
	pthread_spin_destroy(&key_lock);
	pthread_spin_destroy(&stdout_lock);
	return EFAULT;
}

int gui_finalize(void)
{
	if (CONFIG_GUILIB_SIXEL_RAW_MODE) {
		if (tty_raw(false)) {
                        gui_perror("tty disable raw mode fail");
                        return EFAULT;
		}
	}

	locator_enable(false);
	string_write(STDOUT_FILENO, ESCAPE_SHOW_CURSOR);
        return 0;
}

struct gui_window *gui_alloc(void)
{
        return malloc(sizeof(struct gui_window));
}

int gui_create(struct gui_window *window, unsigned int width, unsigned int height)
{
	SIXELSTATUS status;
        int err;

	g_window = window;

	window->width = width;
	window->height = height;
	window->length = width * height;
        window->raw_pixels = NULL;
        window->sixel_output = NULL;
        window->sixel_dither = NULL;
	memset(g_pressed_keys, 0, 255);

	window->raw_pixels = malloc(sizeof(unsigned) * window->length);
	memset(window->raw_pixels, 0, window->length * sizeof(unsigned));
	if (window->raw_pixels == NULL) {
		gui_perror("out of memory");
                err = ENOMEM;
		goto fail;
	}

	status = sixel_output_new(&window->sixel_output, sixel_write, stdout, NULL);
	if (SIXEL_FAILED(status)) {
		gui_perror("create sixel output fail");
                err = EFAULT;
		goto fail;
	}

	status = sixel_dither_new(&window->sixel_dither, SIXEL_PALETTE_MAX, NULL);
	if (SIXEL_FAILED(status)) {
		gui_perror("sixel sixel dither fail");
                err = EFAULT;
		goto fail;
	}

	sixel_output_set_encode_policy(window->sixel_output, SIXEL_ENCODEPOLICY_FAST);
	return 0;

fail:
        gui_destroy(window);
        return err;
}

int gui_destroy(struct gui_window *window)
{
	free(window->raw_pixels);
        sixel_output_destroy(window->sixel_output);
        sixel_dither_destroy(window->sixel_dither);
        return 0;
}

unsigned int gui_width(struct gui_window *window)
{
	return window->width;
}

unsigned int gui_height(struct gui_window *window)
{
	return window->height;
}

void gui_set_pixel(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
#ifdef CONFIG_GUILIB_DEBUG
	if (x >= gui_width(window) || y >= gui_height(window)) {
		fprintf(stderr, "\r\ngui_set_pixel out of bounds: %u %u\n", x, y);
	}
#endif
	gui_set_pixel_raw(window, (unsigned long)y * window->width + x, color);
}

int gui_set_pixel_safe(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
	unsigned long i = (unsigned long)y * window->width + x;

	if (i < window->length) {
		gui_set_pixel_raw(window, i, color);
		return 0;
	} else {
		return EINVAL;
	}
}

void gui_set_pixel_raw(struct gui_window *window, unsigned long i, unsigned color)
{
	window->raw_pixels[i] = color;
}

unsigned *gui_raw_pixels(struct gui_window *window)
{
	return window->raw_pixels;
}

int gui_draw(struct gui_window *window)
{
	SIXELSTATUS status;

	fps_relax();

	status = sixel_dither_initialize(
		window->sixel_dither, (void *)window->raw_pixels, window->width,
		window->height, SIXEL_PIXELFORMAT_BGRA8888, SIXEL_LARGE_AUTO,
		SIXEL_REP_AUTO, SIXEL_QUALITY_FULL);
        if (SIXEL_FAILED(status)) {
                gui_perror("sixel dither initialize fail");
                return EFAULT;
        }

	pthread_spin_lock(&stdout_lock);
	if (!CONFIG_GUILIB_SIXEL_NO_DRAW) {
		string_write(STDOUT_FILENO, ESCAPE_CLEAR_SCREEN);
	}
	status = sixel_encode((void *)window->raw_pixels, window->width,
			      window->height, 0, window->sixel_dither,
			      window->sixel_output);
	if (SIXEL_FAILED(status)) {
                gui_perror("sixel encode fail");
	        pthread_spin_unlock(&stdout_lock);
                return EFAULT;
	}
	string_write(STDOUT_FILENO, "\r\n");
	pthread_spin_unlock(&stdout_lock);

        return 0;
}

void gui_key_hook(struct gui_window *window, key_hook_t hook)
{
	window->callback = hook;
}

void gui_mouse(struct gui_window *window, int *x, int *y)
{
	(void)window;
	*x = READ_ONCE(g_mouse_x);
	*y = READ_ONCE(g_mouse_y);
}

void gui_wfi(struct gui_window *window)
{
	double now = time_now();
	(void)window;

	while (now > READ_ONCE(g_last_interrupt_time)) {
		usleep(10000);
	}
	return;
}

bool gui_pressed(struct gui_window *window, unsigned char keycode)
{
	(void)window;
	return READ_ONCE(g_pressed_keys[keycode]);
}

bool gui_closed(struct gui_window *window)
{
	return gui_pressed(window, GUI_CLOSED);
}

