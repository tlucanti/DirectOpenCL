
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>

#define for_each_pixel(x, y, i, width, height)	\
	for (y = 0; y < height; y++, i++)	\
		for (x = 0; x < width; x++, i++)

struct gui_window {
	void *PY_window;
	unsigned int *raw_pixels;
};

typedef void (*key_hook_t)(struct gui_window *window, int keycode, bool pressed);

typedef void *(*_PY_window_constructor_t)(unsigned int, unsigned int);
typedef void (*_PY_window_draw_t)(void *, unsigned int *);
typedef void (*_PY_window_key_hook_t)(void *, void *);
typedef void (*_PY_window_destructor_t)(void *);

_PY_window_constructor_t _PY_window_constructor = NULL;
_PY_window_draw_t _PY_window_draw = NULL;
_PY_window_key_hook_t _PY_window_key_hook = NULL;
_PY_window_destructor_t _PY_window_destructor = NULL;

void _set_PY_window_constructor(_PY_window_constructor_t func)
{
	printf("c: constructor set to %p\n", func);
	_PY_window_constructor = func;
}

void _set_PY_window_draw(_PY_window_draw_t func)
{
	printf("c: draw callback set to %p\n", func);
	_PY_window_draw = func;
}

void _set_PY_window_key_hook(_PY_window_key_hook_t func)
{
	printf("c: key hook set to %p\n", func);
	_PY_window_key_hook = func;
}

void _set_PY_window_destructor(_PY_window_destructor_t func)
{
	printf("c: destructor set to %p\n", func);
	_PY_window_destructor = func;
}

void gui_bootstrap(void)
{
}

void gui_create(struct gui_window *window, unsigned int width, unsigned int height)
{
	printf("c: calling window constructor\n");
	window->PY_window = _PY_window_constructor(width, height);
	printf("c: created window %p\n", window->PY_window);
	window->raw_pixels = malloc(sizeof(unsigned int) * width * height);
}

void gui_draw(const struct gui_window *window)
{
	//printf("c: calling draw callback: %p\n", window->raw_pixels);
	_PY_window_draw(window->PY_window, window->raw_pixels);
}

void gui_key_hook(struct gui_window *window, key_hook_t hook)
{
	_PY_window_key_hook(window->PY_window, hook);
}

void gui_destroy(struct gui_window *window)
{
	printf("c: destroying window %p\n", window->PY_window);
	_PY_window_destructor(window->PY_window);
	free(window->raw_pixels);

	window->PY_window = NULL;
	window->raw_pixels = NULL;
}

void callback(struct gui_window *window, int keycode, bool pressed)
{
	printf("window (%p): keyboard event %d\n", window, keycode);
}

int main()
{
	struct gui_window window;
	const int width = 800, height = 600;

	srand(0);

	gui_bootstrap();

	gui_create(&window, width, height);
	gui_key_hook(&window, callback);

	while (1) {
		const int w = 1;
		unsigned int color = 0xff << (8 * (rand() % 3));
		int i = 0;

		printf("drawing with color %x\n", color);
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (x < w || y < w || height - y <= w || width - x <= w) {
					window.raw_pixels[i] = color;
				} else {
					window.raw_pixels[i] = 0;
				}
				i++;
			}
		}
		gui_draw(&window);
		break;
	}

	gui_destroy(&window);
}

