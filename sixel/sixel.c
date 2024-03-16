
#include <sixel.h>

#include <stdio.h>
#include <stdlib.h>

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

	draw_circle(&canvas, 100, 100, 40, 0xff);

	status = sixel_encode(canvas.buf, canvas.width, canvas.height, 0, dither, output);
	if (SIXEL_FAILED(status)) {
		printf("sixel_encode() fail\n");
		goto end;
	}

end:
	return 0;
}
