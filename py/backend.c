
#include <stdbool.h>
#include <malloc.h>

#include "../backend.h"
#include "../common.h"

#define for_each_pixel(x, y, i, width, height)	\
	for (y = 0; y < height; y++, i++)	\
		for (x = 0; x < width; x++, i++)

typedef void *(*PY_window_constructor_t)(unsigned int, unsigned int, void *);
typedef void (*PY_window_draw_t)(void *, unsigned int *);
typedef void (*PY_window_key_hook_t)(void *, key_hook_t);
typedef void (*PY_window_wfi_t)(void *);
typedef void (*PY_window_destructor_t)(void *);

PY_window_constructor_t PY_window_constructor = NULL;
PY_window_draw_t PY_window_draw = NULL;
PY_window_key_hook_t PY_window_key_hook = NULL;
PY_window_wfi_t PY_window_wfi = NULL;
PY_window_destructor_t PY_window_destructor = NULL;

__noinline __used
void _set_PY_window_constructor(PY_window_constructor_t func)
{
	PY_window_constructor = func;
}

__noinline __used
void _set_PY_window_draw(PY_window_draw_t func)
{
	PY_window_draw = func;
}

__noinline __used
void _set_PY_window_key_hook(PY_window_key_hook_t func)
{
	PY_window_key_hook = func;
}

__noinline __used
void _set_PY_window_wfi(PY_window_wfi_t func)
{
	PY_window_wfi = func;
}

__noinline __used
void _set_PY_window_destructor(PY_window_destructor_t func)
{
	PY_window_destructor = func;
}

void gui_bootstrap(void)
{
}

void gui_create(struct gui_window *window, unsigned int width, unsigned int height)
{
	window->__PY_window = PY_window_constructor(width, height, window);
	window->__width = width;
	window->__height = height;
	window->__length = (unsigned long)width * height;
	window->__raw_pixels = malloc(sizeof(unsigned int) * window->__length);
}

void gui_draw(const struct gui_window *window)
{
	PY_window_draw(window->__PY_window, window->__raw_pixels);
}

void gui_key_hook(struct gui_window *window, key_hook_t hook)
{
	PY_window_key_hook(window->__PY_window, hook);
}

void gui_wfi(struct gui_window *window)
{
	PY_window_wfi(window->__PY_window);
}

void gui_destroy(struct gui_window *window)
{
	printf("c: destroying window %p\n", window->__PY_window);
	PY_window_destructor(window->__PY_window);
	free(window->__raw_pixels);

	window->__PY_window = NULL;
	window->__raw_pixels = NULL;
}

void gui_set_pixel_raw(struct gui_window *window, unsigned long i, unsigned color)
{
	window->__raw_pixels[i] = color;
}

void gui_set_pixel(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
	gui_set_pixel_raw(window, (unsigned long)y * window->__width + x, color);
}

int gui_set_pixel_raw_safe(struct gui_window *window, unsigned long i, unsigned color)
{
	if (i < window->__length) {
		window->__raw_pixels[i] = color;
		return 0;
	} else {
		return 1;
	}
}

int gui_set_pixel_safe(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
	return gui_set_pixel_raw_safe(window, (unsigned long)y * window->__width + x, color);
}

