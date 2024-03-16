
#include <sixel.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

struct canvas {
	int width;
	int height;
	unsigned char *buf;
};

static int sixel_write(char *data, int size, void *out)
{
	return fwrite(data, 1, size, (FILE *)out);
}

void set_pixel(struct canvas *canvas, int x, int y, unsigned char color)
{
	canvas->buf[canvas->width * y + x] = color;
}

#define square(x) ((x) * (x))

void draw_circle(struct canvas *canvas, int x, int y, int radius, unsigned char color)
{
	for (int yy = y - radius / 2; yy < y + radius / 2; yy++) {
		for (int xx = x - radius / 2; xx < x + radius / 2; xx++) {
			if (square(xx - x) + square(yy - y) <= radius) {
				set_pixel(canvas, xx, yy, color);
			}
		}
	}
}

void draw_borders(struct canvas *canvas, unsigned char color)
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

	canvas.width = 400;
	canvas.height = 300;
	canvas.buf = malloc(sizeof(unsigned char) * canvas.width * canvas.height);
	if (canvas.buf == NULL) {
		printf("canvas alloc fail\n");
		goto end;
	}

	status = sixel_output_new(&output, sixel_write, stdout, NULL);
	if (SIXEL_FAILED(status)) {
		printf("sixel_output_new() fail\n");
		goto end;
	}

	dither = sixel_dither_get(SIXEL_BUILTIN_G8);
	sixel_dither_set_pixelformat(dither, SIXEL_PIXELFORMAT_G8);

	int x = 100, y = 100;
	while (true) {
		//draw_circle(&canvas, x, y, 40, 0x00);
		x = rand() % 200 + 40;
		y = rand() % 200 + 40;
		draw_circle(&canvas, x, y, 40, rand() % 256);

		printf("\e[0;0H");
		fflush(stdout);
		draw_borders(&canvas, 0xff);

		status = sixel_encode(canvas.buf, canvas.width, canvas.height, 0, dither, output);
		if (SIXEL_FAILED(status)) {
			printf("sixel_encode() fail\n");
			goto end;
		}

		printf("fps: %f\n", get_fps());
		usleep(10000);
	}

end:
	return 0;
}
