
#include <stdguilib.h>

#include <time.h>

static inline long square(long x)
{
	return x * x;
}

void gui_draw_circle(struct gui_window *window, unsigned xu, unsigned yu, unsigned ru, unsigned color)
{
	int x = xu;
	int y = yu;
	int radius = ru;

	for (int yy = y - radius / 2; yy < (int)(y + radius / 2); yy++) {
		for (int xx = x - radius / 2; xx < (int)(x + radius / 2); xx++) {
			if (square(xx - x) + square(yy - y) <= radius) {
				gui_set_pixel_safe(window, xx, yy, color);
			}
		}
	}
}

void gui_draw_borders(struct gui_window *window, unsigned width, unsigned color)
{
	for (unsigned x = 0; x < gui_width(window); x++) {
		for (unsigned y = 0; y < gui_height(window); y++) {
			if (x < width || x >= gui_width(window) - width ||
			    y < width || y >= gui_height(window) - width) {
				gui_set_pixel(window, x, y, color);
			}
		}
	}
}

float gui_get_fps(void)
{
	static struct timespec ts = {};
	time_t prev_s = ts.tv_sec;
	long prev_ns = ts.tv_nsec;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return 1.f / ((ts.tv_sec - prev_s) + (ts.tv_nsec - prev_ns) * 1e-9f);
}

