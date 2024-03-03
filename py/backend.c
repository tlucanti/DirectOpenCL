
#include <stdio.h>
#include <malloc.h>

#define for_each_pixel(x, y, i, width, height)	\
	for (y = 0; y < height; y++, i++)	\
		for (x = 0; x < width; x++, i++)

struct gui_window {
	void *PY_window;
	unsigned int *raw_pixels;
};

typedef void *(*_PY_window_constructor_t)(unsigned int, unsigned int);
typedef void *(*_PY_window_draw_t)(void *, unsigned int *);
typedef void *(*_PY_window_destructor_t)(void *);

_PY_window_constructor_t _PY_window_constructor = NULL;
_PY_window_draw_t _PY_window_draw = NULL;
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

void _set_PY_window_destructor(_PY_window_destructor_t func)
{
	printf("c: destructor set to %p\n", func);
	_PY_window_destructor = func;
}

void gui_bootstrap(void)
{
}

void gui_create_window(struct gui_window *window, unsigned int width, unsigned int height)
{
	printf("c: calling window constructor\n");
	window->PY_window = _PY_window_constructor(width, height);
	printf("c: created window %p\n", window->PY_window);
	window->raw_pixels = malloc(sizeof(unsigned int) * width * height);
}

void gui_window_draw(const struct gui_window *window)
{
	printf("c: calling draw callback: %p\n", window->raw_pixels);
	_PY_window_draw(window->PY_window, window->raw_pixels);
}

void gui_window_destroy(struct gui_window *window)
{
	printf("c: destroying window %p\n", window->PY_window);
	_PY_window_destructor(window->PY_window);
	free(window->raw_pixels);

	window->PY_window = NULL;
	window->raw_pixels = NULL;
}

int main()
{
	struct gui_window window;

	gui_bootstrap();

	gui_create_window(&window, 10, 10);

	int i = 0;
	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 10; x++) {
			if (x == 0 || y == 0 || x == 9 || y == 9) {
				window.raw_pixels[i] = 0xff;
			} else {
				window.raw_pixels[i] = 0x1;
			}
			i++;
		}
	}

	gui_window_draw(&window);
	gui_window_destroy(&window);
}

