
#include <guilib.h>
#include <stdguilib.h>
#include <stdbool.h>
#include <stdio.h>

static int gx, gy;
static int dx = 0, dy = 0;
static bool mouse1;
static int nr_pressed = 0;

static void callback(struct gui_window *window, int keycode, bool pressed)
{
	const int step = pressed ? 10 : -10;
	(void)window;

	nr_pressed += pressed ? 1 : -1;
	switch (keycode) {
		case 'd':
			dx += step;
			break;
		case 'a':
			dx -= step;
			break;
		case 'w':
			dy -= step;
			break;
		case 's':
			dy += step;
			break;
		case MOUSE_LEFT:
			mouse1 = pressed;
			break;
	}
}

int main()
{
	struct gui_window *window;
	const int width = 800, height = 600;
	int mx1, my1, mx2, my2;

	gui_bootstrap();
        window = gui_alloc();
	gui_create(window, width, height);
	gui_key_hook(window, callback);

	gx = width / 2;
	gy = height / 2;
	gui_mouse(window, &mx1, &my1);

	while (true) {
		gui_draw_borders(window, 2, COLOR_RED);

		gui_draw_circle(window, gx, gy, 20, COLOR_BLACK);
		gx += dx;
		gy += dy;
		gui_draw_circle(window, gx, gy, 20, COLOR_BLUE);

		gui_mouse(window, &mx2, &my2);
		if (mouse1) {
			gui_draw_line(window, mx1, my1, mx2, my2, COLOR_GREEN);
		}
		mx1 = mx2;
		my1 = my2;

		gui_draw(window);

		printf("fps: %f\r\n", gui_get_fps());
	}
}

