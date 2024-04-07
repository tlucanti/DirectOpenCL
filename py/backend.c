
#include <guilib.h>

typedef void *(*PY_window_constructor_t)(unsigned int, unsigned int, void *);
typedef void (*PY_window_destructor_t)(void *);
typedef void (*PY_window_draw_t)(void *, unsigned int *);
typedef void (*PY_window_key_hook_t)(void *, key_hook_t);
typedef void (*PY_window_wfi_t)(void *);
typedef void (*PY_window_mouse_t)(void *, int *, int *);

__used PY_window_constructor_t PY_window_constructor = NULL;
__used PY_window_destructor_t PY_window_destructor = NULL;
__used PY_window_draw_t PY_window_draw = NULL;
__used PY_window_key_hook_t PY_window_key_hook = NULL;
__used PY_window_wfi_t PY_window_wfi = NULL;
__used PY_window_mouse_t PY_window_mouse = NULL;

struct gui_window {
	void *PY_window;
	unsigned int *raw_pixels;
	unsigned int width;
	unsigned int height;
	unsigned long length;
};

noinline __used
void _set_PY_window_constructor(PY_window_constructor_t func)
{
	PY_window_constructor = func;
}

noinline __used
void _set_PY_window_destructor(PY_window_destructor_t func)
{
	PY_window_destructor = func;
}

noinline __used
void _set_PY_window_draw(PY_window_draw_t func)
{
	PY_window_draw = func;
}

noinline __used
void _set_PY_window_key_hook(PY_window_key_hook_t func)
{
	PY_window_key_hook = func;
}

noinline __used
void _set_PY_window_wfi(PY_window_wfi_t func)
{
	PY_window_wfi = func;
}

noinline __used
void _set_PY_window_mouse(PY_window_mouse_t func)
{
        PY_window_mouse = func;
}

int gui_bootstrap(void)
{
        return 0;
}

int gui_finalize(void)
{
        return 0;
}

struct gui_window *gui_alloc(void)
{
        return malloc(sizeof(struct gui_window));
}

int gui_create(struct gui_window *window, unsigned int width, unsigned int height)
{
	window->PY_window = PY_window_constructor(width, height, window);
	window->width = width;
	window->height = height;
	window->length = (unsigned long)width * height;
	window->raw_pixels = malloc(sizeof(unsigned int) * window->length);

        if (window->PY_window == NULL) {
                gui_perror("python window constructor fail");
                return EFAULT;
        } else if (window->raw_pixels == NULL) {
                gui_perror("out of memory");
                return ENOMEM;
        }
        return 0;
}

int gui_destroy(struct gui_window *window)
{
	printf("c: destroying window %p\n", window->PY_window);
	PY_window_destructor(window->PY_window);
	free(window->raw_pixels);

	window->PY_window = NULL;
	window->raw_pixels = NULL;
        return 0;
}

unsigned int gui_width(struct gui_window *window)
{
        return window->width;
}

unsigned int gui_height(struct gui_window *window)
{
        return window->height;
}

void gui_set_pixel(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
	gui_set_pixel_raw(window, (unsigned long)y * window->width + x, color);
}

void gui_set_pixel_raw(struct gui_window *window, unsigned long i, unsigned color)
{
	window->raw_pixels[i] = color;
}

int gui_set_pixel_safe(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
	unsigned long i = (unsigned long)y * window->width + x;

	if (i < window->length) {
		gui_set_pixel_raw(window, i, color);
		return 0;
	} else {
		return EINVAL;
	}
}

unsigned *gui_raw_pixels(struct gui_window *window)
{
        return window->raw_pixels;
}

int gui_draw(struct gui_window *window)
{
	PY_window_draw(window->PY_window, window->raw_pixels);
        return 0;
}

void gui_key_hook(struct gui_window *window, key_hook_t hook)
{
	PY_window_key_hook(window->PY_window, hook);
}

void gui_mouse(struct gui_window *window, int *x, int *y)
{
        PY_window_mouse(window->PY_window, x, y);
}

void gui_wfi(struct gui_window *window)
{
	PY_window_wfi(window->PY_window);
}

