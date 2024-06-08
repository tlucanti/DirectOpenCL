
#include <stdguilib.h>

#include <time.h>

#define square _square
#define abs _abs

static inline long _square(long x)
{
	return x * x;
}

static inline int _abs(int x)
{
	return x > 0 ? x : -x;
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

void gui_draw_line(struct gui_window *window, unsigned x0, unsigned y0,
		   unsigned x1, unsigned y1, unsigned color)
{
	int dx, dy;
	int sx, sy;
	int err;

	dx = abs(x1 - x0);
	dy = -abs(y1 - y0);

	sx = x0 < x1 ? 1 : -1;
	sy = y0 < y1 ? 1 : -1;
	err = dx + dy;

	while (true) {
		if (x0 > gui_width(window) && y0 > gui_height(window)) {
			break;
		}
		gui_set_pixel_safe(window, x0, y0, color);

		if (x0 == x1 && y0 == y1) {
			break;
		}

		if (err * 2 > dy) {
			if (x0 == x1) {
				break;
			}
			err += dy;
			x0 += sx;
		}
		if (err * 2 <= dx) {
			if (y0 == y1) {
				break;
			}
			err += dx;
			y0 += sy;
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

