
#include <sixel.h>
#include <guilib.h>
#include <stdguilib.h>

#include <termios.h>
#include <pthread.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#ifndef CONFIG_SIXEL_RAW_MODE
# define CONFIG_SIXEL_RAW_MODE true
#endif

#ifndef CONFIG_KEYBOARD_ENABLE
# define CONFIG_KEYBOARD_ENABLE true
#endif

#define DEBUG

#define ESCAPE_CLEAR_SCREEN "\033[0;0H"
#define ESCAPE_HIDE_CURSOR "\033[?25l"
#define ESCAPE_SHOW_CURSOR "\033[?25h"
#define ESCAPE_ENABLE_LOCATOR "\033[?1003h" "\033[?1015h" "\033[?1006h" "\033[?1016h"
#define ESCAPE_DISABLE_LOCATOR "\e[?1000l"

//#define WANR_UNKNOWN_ESCAPE() fprintf(stderr, "unknow escape sequence\n")
#define WANR_UNKNOWN_ESCAPE() /* empty */

#define DELTA 0.1f
#define MAX_FPS_RELAX 1000

static struct termios g_term_info;
static volatile int g_mouse_x = 0;
static volatile int g_mouse_y = 0;
static volatile key_hook_t g_callback = NULL;
static struct gui_window *g_window = NULL;
static volatile bool wfi_flag;
static pthread_spinlock_t stdout_lock;
static pthread_spinlock_t key_lock;
static volatile bool key_reader_thread_should_stop = false;

struct key_metadata {
	double last_press_time;
	int keycode;
	bool released;
};

static void fps_relax(void)
{
	static struct timespec ts = {};
	time_t prev_s = ts.tv_sec;
	long prev_ns = ts.tv_nsec;
	float fps;

	return;

	do {
		clock_gettime(CLOCK_MONOTONIC, &ts);
		fps = 1.f / ((ts.tv_sec - prev_s) + (ts.tv_nsec - prev_ns) * 1e-9f);
		//usleep(5000);
	} while (fps > MAX_FPS_RELAX);
}

static void set_wfi_flag(void)
{
	pthread_spin_lock(&key_lock);
	wfi_flag = true;
	pthread_spin_unlock(&key_lock);
}

static void key_callback(int keycode, bool pressed)
{
	if (g_callback == NULL) {
		return;
	}

	g_callback(g_window, keycode, pressed);
	set_wfi_flag();
}

static void string_write(int fd, const char *s)
{
	write(fd, s, strlen(s));
}

static void locator_enable(bool enable)
{
	if (enable) {
		string_write(STDOUT_FILENO, ESCAPE_ENABLE_LOCATOR);
	} else {
		string_write(STDOUT_FILENO, ESCAPE_DISABLE_LOCATOR);
	}
}

static int tty_init(void)
{
	if (tcgetattr(STDIN_FILENO, &g_term_info) != 0) {
		perror("tty_init() failed");
		return EINVAL;
	}
	return 0;
}

static int tty_raw(bool enable)
{
	struct termios raw_mode;

	if (!enable) {
		if (tcsetattr(STDIN_FILENO, TCSANOW, &g_term_info) != 0) {
			perror("tty restore failed: ");
			return EINVAL;
		}
		return 0;
	}

	raw_mode = g_term_info;
	cfmakeraw(&raw_mode);

	if (tcsetattr(STDIN_FILENO, TCSANOW, &raw_mode) != 0) {
		perror("tty set raw failed: ");
		tty_raw(false);
		return EINVAL;
	}

	return 0;
}

static int sixel_write(char *data, int size, void *arg)
{
	(void)arg;
	write(STDOUT_FILENO, data, size);
	return 0;
}

static double time_now()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9f;
}

static void *release_checker_thread(void *arg)
{
	struct key_metadata *meta = arg;

	usleep(DELTA * 1e6f);
	pthread_spin_lock(&key_lock);
	if (time_now() - meta->last_press_time > DELTA && !meta->released) {
		meta->released = true;
		pthread_spin_unlock(&key_lock);
		g_callback(g_window, meta->keycode, false);
		set_wfi_flag();
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

	if (g_callback == NULL) {
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
		pthread_spin_lock(&stdout_lock);
		fprintf(stderr, "\nkey overflow\n");
		pthread_spin_unlock(&stdout_lock);
		abort();
	}

	time = time_now();
	do_callback = (time - meta[keycode].last_press_time > DELTA);
	meta[keycode].last_press_time = time;
	if (pthread_create(&tid, NULL, release_checker_thread, &meta[keycode])) {
		perror("release_checker_thread spawn fail");
		abort();
	}
	if (pthread_detach(tid)) {
		perror("release_checker_thread detach fail");
		abort();
	}

	if (do_callback) {
		pthread_spin_lock(&key_lock);
		meta[keycode].released = false;
		pthread_spin_unlock(&key_lock);
		g_callback(g_window, keycode, true);
		set_wfi_flag();
	}
}

static void *key_reader_thread(void *arg)
{
	int button, x, y;
	char c, type;
	int success;
	bool pressed;

	(void)arg;

	while (!key_reader_thread_should_stop) {
		c = getc(stdin);
		if (c == EOF) {
			goto read_fail;
		}

		if (!CONFIG_SIXEL_RAW_MODE && c == '\n') {
			continue;
		}

		if (c == '\e') {
			c = getc(stdin);
			if (c == EOF) {
				goto read_fail;
			} else if (c != '[') {
				pthread_spin_lock(&stdout_lock);
				WANR_UNKNOWN_ESCAPE();
				pthread_spin_unlock(&stdout_lock);
				continue;
			}

			c = getc(stdin);
			if (c == EOF) {
				goto read_fail;
			}

			switch (c) {
			case 65:
				key_reader_event_tracker(KEY_UP);
				break;
			case 66:
				key_reader_event_tracker(KEY_DOWN);
				break;
			case 67:
				key_reader_event_tracker(KEY_RIGHT);
				break;
			case 68:
				key_reader_event_tracker(KEY_LEFT);
				break;
			case '<':
				success = scanf("%d;%d;%d%c", &button, &x, &y, &type);
				if (success == EOF) {
					goto read_fail;
				} else if (success != 4) {
					pthread_spin_lock(&stdout_lock);
					WANR_UNKNOWN_ESCAPE();
					pthread_spin_unlock(&stdout_lock);
					continue;
				}

				switch (button) {
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
					if (type == 'M') {
						// mouse button pressed
						pressed = true;
					} else if (type == 'm') {
						// mouse button released
						pressed = false;
					} else {
						pthread_spin_lock(&stdout_lock);
						WANR_UNKNOWN_ESCAPE();
						pthread_spin_unlock(&stdout_lock);
						continue;
					}

					key_callback(button, pressed);
					break;
				case 35:
					// mouse move
					break;
				case 64:
					key_callback(MOUSE_SCROLL_UP, true);
					break;
				case 65:
					key_callback(MOUSE_SCROLL_DOWN, true);
					break;
				default:
					WANR_UNKNOWN_ESCAPE();
				}
				g_mouse_x = x;
				g_mouse_y = y;
			}
		} else if (c == 'x') {
			pthread_spin_lock(&stdout_lock);
			gui_finalize();
			pthread_spin_unlock(&stdout_lock);
			set_wfi_flag();
			exit(0);
		} else {
			key_reader_event_tracker(c);
		}
	}

	return NULL;

read_fail:
	pthread_spin_lock(&stdout_lock);
	perror("key_reader_thread: stdin read error");
	pthread_spin_unlock(&stdout_lock);
	abort();
}

void gui_bootstrap(void)
{
	pthread_t tid;

	if (pthread_spin_init(&stdout_lock, false)) {
		perror("stdout_lock init fail");
		goto fail;
	}
	if (pthread_spin_init(&key_lock, false)) {
		perror("key_lock init fail");
		goto fail;
	}

	if (CONFIG_KEYBOARD_ENABLE) {
		if (pthread_create(&tid, NULL, key_reader_thread, NULL)) {
			perror("key_reader_thread spawn fail");
			goto fail;
		}
		locator_enable(true);
	}

	if (CONFIG_SIXEL_RAW_MODE) {
		if (tty_init()) {
			goto fail;
		}
		if (tty_raw(true)) {
			goto fail;
		}
	}

	pthread_spin_lock(&stdout_lock);
	string_write(STDOUT_FILENO, ESCAPE_HIDE_CURSOR);
	pthread_spin_unlock(&stdout_lock);

	return;
fail:
	key_reader_thread_should_stop = true;
	pthread_spin_destroy(&key_lock);
	pthread_spin_destroy(&stdout_lock);
	abort();
}

void gui_finalize(void)
{
	if (CONFIG_SIXEL_RAW_MODE) {
		if (tty_raw(false)) {
			abort();
		}
	}
	locator_enable(false);
	string_write(STDOUT_FILENO, ESCAPE_SHOW_CURSOR);
}

void gui_create(struct gui_window *window, unsigned int width, unsigned int height)
{
	SIXELSTATUS status;

	window->__width = width;
	window->__height = height;
	window->__length = width * height;
	window->__raw_pixels = malloc(sizeof(unsigned int) * window->__length);
	g_window = window;

	if (window->__raw_pixels == NULL) {
		perror("window buffer alloc fail");
		goto fail;
	}

	status = sixel_output_new(&window->__output, sixel_write, stdout, NULL);
	if (SIXEL_FAILED(status)) {
		perror("sixel_output_new() fail");
		goto fail;
	}

	status = sixel_dither_new(&window->__dither, SIXEL_PALETTE_MAX, NULL);
	if (SIXEL_FAILED(status)) {
		perror("sixel_dither_new() fail");
		goto fail;
	}

	sixel_output_set_encode_policy(window->__output, SIXEL_ENCODEPOLICY_FAST);
	return;
fail:
	gui_finalize();
	abort();
}

void gui_destroy(struct gui_window *window)
{
	free(window->__raw_pixels);
}

unsigned int gui_width(const struct gui_window *window)
{
	return window->__width;
}

unsigned int gui_height(const struct gui_window *window)
{
	return window->__height;
}

void gui_set_pixel(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
#ifdef DEBUG
	if (x >= gui_width(window) || y >= gui_height(window)) {
		fprintf(stderr, "\r\ngui_set_pixel out of bounds: %u %u\n", x, y);
	}
#endif
	gui_set_pixel_raw(window, (unsigned long)y * window->__width + x, color);
}

int gui_set_pixel_safe(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
	unsigned long i = (unsigned long)y * window->__width + x;

	if (i < window->__length) {
		gui_set_pixel_raw(window, i, color);
		return 0;
	} else {
		return 1;
	}
}

void gui_set_pixel_raw(struct gui_window *window, unsigned long i, unsigned color)
{
	window->__raw_pixels[i] = color;
}

unsigned *gui_raw_pixels(const struct gui_window *window)
{
	return window->__raw_pixels;
}

void gui_draw(const struct gui_window *window)
{
	SIXELSTATUS status;

	fps_relax();

	sixel_dither_initialize(window->__dither, (void *)window->__raw_pixels,
				window->__width, window->__height,
				SIXEL_PIXELFORMAT_BGRA8888, SIXEL_LARGE_AUTO,
				SIXEL_REP_AUTO, SIXEL_QUALITY_FULL);

	pthread_spin_lock(&stdout_lock);
	string_write(STDOUT_FILENO, ESCAPE_CLEAR_SCREEN);
	status = sixel_encode((void *)window->__raw_pixels, window->__width,
			      window->__height, 0, window->__dither,
			      window->__output);
	string_write(STDOUT_FILENO, "\r\n");
	pthread_spin_unlock(&stdout_lock);

	if (SIXEL_FAILED(status)) {
		fprintf(stderr, "sixel_encode() fail\n");
		gui_finalize();
		abort();
	}
}

void gui_key_hook(struct gui_window *window, key_hook_t hook)
{
	(void)window;
	g_callback = hook;
}

void gui_mouse(const struct gui_window *window, int *x, int *y)
{
	(void)window;
	*x = g_mouse_x;
	*y = g_mouse_y;
}

void gui_wfi(struct gui_window *window)
{
	(void)window;

	pthread_spin_lock(&key_lock);
	wfi_flag = false;
	pthread_spin_unlock(&key_lock);

	while (!wfi_flag) {
		usleep(100000);
	}
	return;
}

