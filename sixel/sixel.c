
#include <sixel.h>

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

int main()
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
		goto end;
	}

	status = sixel_output_new(&output, sixel_write, stdout, NULL);
	if (SIXEL_FAILED(status)) {
		printf("sixel_output_new() fail\n");
		goto end;
	}

	sixel_dither_new(&dither, SIXEL_PALETTE_MAX, NULL);

	int y = 50;
	int col = 0;

next:
	for (int x = 0; x < 256; x++) {
		draw_circle(&canvas, x * 4, y, 40, x << col);


		printf("\e[0;0H");
		fflush(stdout);
		draw_borders(&canvas, 0xffffff);

		sixel_dither_initialize(dither, canvas.buf, canvas.width, canvas.height,
					SIXEL_PIXELFORMAT_RGBA8888,
					SIXEL_LARGE_AUTO,
					SIXEL_REP_AUTO,
					SIXEL_QUALITY_AUTO);
		status = sixel_encode((void *)canvas.buf, canvas.width, canvas.height, 0, dither, output);
		if (SIXEL_FAILED(status)) {
			printf("sixel_encode() fail\n");
			goto end;
		}

		printf("fps: %f\n", get_fps());
		usleep(5000);
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

end:
	return 0;
}
