
#include "../backend.h"
#include <stdbool.h>
#include <time.h>
#include <stdio.h>

#define STEP 10
static int dx, dy;

static void callback(struct gui_window *window, unsigned keycode, bool pressed)
{
	const int delta = pressed ? STEP : 0;
	(void)window;

	switch (keycode) {
	case KEY_W:
		dy = -delta;
		break;
	case KEY_S:
		dy = delta;
		break;
	case KEY_A:
		dx = -delta;
		break;
	case KEY_D:
		dx = delta;
		break;
	}
}

#define square(x) ((x) * (x))

static void draw_circle(struct gui_window *window, int x, int y, int radius, unsigned color)
{
	for (int yy = y - radius / 2; yy < y + radius / 2; yy++) {
		for (int xx = x - radius / 2; xx < x + radius / 2; xx++) {
			if (square(xx - x) + square(yy - y) <= radius) {
				gui_set_pixel_safe(window, xx, yy, color);
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

int main()
{
	struct gui_window window;
	const unsigned width = 800, height = 600;
	int x, y;

	gui_bootstrap();
	gui_create(&window, width, height);
	gui_key_hook(&window, (key_hook_t)callback);

	x = width / 2, y = height / 2;
	while (true) {
		printf("fps: %f\n", get_fps());

		draw_circle(&window, x, y, 20, COLOR_BLACK);
		x = (x + dx + width) % width;
		y = (y + dy + height) % height;
		draw_circle(&window, x, y, 20, COLOR_BLUE);

		gui_draw(&window);
		//gui_wfi(&window);
	}
}


