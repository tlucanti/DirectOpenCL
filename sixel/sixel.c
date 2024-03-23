
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

#ifndef CONFIG_SIXEL_RAW_MODE
# define CONFIG_SIXEL_RAW_MODE true
#endif

#define ESCAPE_CLEAR_SCREEN "\033[0;0H"
#define ESCAPE_ENABLE_LOCATOR "\033[?1003h\033[?1015h\033[?1006h"
#define ESCAPE_DISABLE_LOCATOR "\e[?1000l"

#define DELTA 0.1f

static struct termios g_term_info;
static int g_mouse_x = 0;
static int g_mouse_y = 0;
key_hook_t g_callback = NULL;
struct gui_window *g_window = NULL;

struct key_metadata {
	double last_press_time;
	int keycode;
	pthread_spinlock_t lock;
	bool released;
};

static void locator_enable(bool enable)
{
	if (enable) {
		printf(ESCAPE_ENABLE_LOCATOR);
	} else {
		printf(ESCAPE_DISABLE_LOCATOR);
	}
	fflush(stdout);
}

static void tty_init(void)
{
	if (tcgetattr(STDIN_FILENO, &g_term_info) != 0) {
		perror("tty_init() failed: ");
		abort();
	}
}

static void tty_raw(bool enable)
{
	struct termios raw_mode;

	if (!enable) {
		if (tcsetattr(STDIN_FILENO, TCSANOW, &g_term_info) != 0) {
			perror("tty restore failed: ");
			abort();
		}
		return;
	}

	memcpy(&raw_mode, &g_term_info, sizeof(struct termios));
	cfmakeraw(&raw_mode);

	if (tcsetattr(STDIN_FILENO, TCSANOW, &raw_mode) != 0) {
		perror("tty set raw failed: ");
		tty_raw(false);
		abort();
	}
}

static int sixel_write(char *data, int size, void *arg)
{
	(void)arg;
	fwrite(data, 1, size, stdout);
	fflush(stdout);
	return 0;
}

static double time_now()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9f;
}

static void *release_checker(void *arg)
{
	struct key_metadata *meta = arg;

	usleep(DELTA * 1e6f);
	pthread_spin_lock(&meta->lock);
	if (time_now() - meta->last_press_time > DELTA && !meta->released) {
		meta->released = true;
		pthread_spin_unlock(&meta->lock);
		g_callback(g_window, meta->keycode, false);
	}
	pthread_spin_unlock(&meta->lock);
	return NULL;
}

static void key_tracker(int keycode)
{
	static struct key_metadata meta[256] = {};
	static bool init = false;
	pthread_t tid;
	double time;
	bool do_callback;

	if (!init) {
		init = true;
		for (int i = 0; i < 256; i++) {
			meta[i].released = false;
			meta[i].last_press_time = 0;
			meta[i].keycode = i;
			pthread_spin_init(&meta[i].lock, false);
		}
	}

	if (keycode >= 256) {
		printf("\nkey overflow\n");
		abort();
	}

	time = time_now();
	do_callback = (time - meta[keycode].last_press_time > DELTA);
	meta[keycode].last_press_time = time;
	//printf("last_press_time %f\r\n", time);
	pthread_create(&tid, NULL, release_checker, &meta[keycode]);
	pthread_detach(tid);

	if (do_callback) {
		pthread_spin_lock(&meta[keycode].lock);
		meta[keycode].released = false;
		pthread_spin_unlock(&meta[keycode].lock);
		g_callback(g_window, keycode, true);
	}
}

__attribute__((__noreturn__))
static void *key_reader(void *arg)
{
	int button, x, y;
	char c, type;
	int success;

	(void)arg;

	while (true) {
		success = scanf("%c", &c);
		if (success == 0) {
			printf("\ncannot read from stdin\n");
			abort();
		}

		if (!CONFIG_SIXEL_RAW_MODE && c == '\n') {
			continue;
		}

		if (c == '\e') {
			success = scanf("[<%d;%d;%d%c", &button, &x, &y, &type);
			if (success == 4) {
				if (button == 35) {
					//printf("mouse move to");
				} else if (button == 65) {
					//printf("scroll down at");
				} else if (button == 64) {
					//printf("scroll up at");
				} else if (button <= 7 && type == 'M') {
					//printf("button %d pressed", button);
					if (g_callback != NULL) {
						g_callback(g_window, button, true);
					}
				} else if (button <= 7 && type == 'm') {
					//printf("button %d released at", button);
					if (g_callback != NULL) {
						g_callback(g_window, button, false);
					}
				} else {
					//printf("UNKNOWN EVENT %d %c", button, type);
				}
				//printf(" x=%d y=%d\n", x, y);
				g_mouse_x = x;
				g_mouse_y = y;
			}
		} else if (c == 'x') {
			gui_finalize();
			exit(0);
		} else {
			if (g_callback != NULL) {
				key_tracker(c);
			}
		}
	}
}

void gui_bootstrap(void)
{
	pthread_t tid;

	pthread_create(&tid, NULL, key_reader, NULL);

	if (CONFIG_SIXEL_RAW_MODE) {
		tty_init();
		tty_raw(true);
	}
	locator_enable(true);
}

void gui_finalize(void)
{
	if (CONFIG_SIXEL_RAW_MODE) {
		tty_raw(false);
	}
	locator_enable(false);
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
		fprintf(stderr, "window buffer alloc failed\n");
		gui_finalize();
		abort();
	}

	status = sixel_output_new(&window->__output, sixel_write, stdout, NULL);
	if (SIXEL_FAILED(status)) {
		fprintf(stderr, "sixel_output_new() failed\n");
		gui_finalize();
		abort();
	}

	status = sixel_dither_new(&window->__dither, SIXEL_PALETTE_MAX, NULL);
	if (SIXEL_FAILED(status)) {
		fprintf(stderr, "sixel_dither_new() failed\n");
		gui_finalize();
		abort();
	}

	sixel_output_set_encode_policy(window->__output, SIXEL_ENCODEPOLICY_FAST);
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

void gui_draw(const struct gui_window *window)
{
	SIXELSTATUS status;

	printf(ESCAPE_CLEAR_SCREEN);
	fflush(stdout);

	sixel_dither_initialize(window->__dither, (void *)window->__raw_pixels,
				window->__width, window->__height,
				SIXEL_PIXELFORMAT_RGBA8888, SIXEL_LARGE_AUTO,
				SIXEL_REP_AUTO, SIXEL_QUALITY_FULL);
	status = sixel_encode((void *)window->__raw_pixels, window->__width,
			      window->__height, 0, window->__dither,
			      window->__output);

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
	//usleep(100000);
	return;
}


static int gx, gy;
static void callback(struct gui_window *window, int keycode, bool pressed)
{
	(void)window;
	if (pressed) {
		printf("pressed %d", keycode);
	} else {
		printf("released %d", keycode);
	}
	if (keycode <= 7) {
		int x, y;
		gui_mouse(window, &x, &y);
		printf(" at (%d, %d)", x, y);
	}
	printf("\r\n");
}

int main()
{
	struct gui_window window;
	const int width = 400, height = 300;
	//int x = 0, y = 0;

	gui_bootstrap();
	gui_create(&window, width, height);
	gui_key_hook(&window, callback);

	gx = width / 2;
	gy = height / 2;
	while (true) {
		gui_wfi(&window);
	}

	//while (true) {
	//	gui_draw_circle(&window, x, y, 20, COLOR_BLACK);
	//	x = gx;
	//	y = gy;
	//	gui_draw_circle(&window, x, y, 20, COLOR_WHITE);
	//	gui_draw(&window);
	//	gui_wfi(&window);
	//}
}

