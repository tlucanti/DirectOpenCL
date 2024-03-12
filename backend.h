
#ifndef GUILIB_BACKEND_H
#define GUILIB_BACKEND_H

#include <stdbool.h>

struct gui_window;

typedef void (*key_hook_t)(struct gui_window *window, int keycode, bool pressed);

void gui_bootstrap(void);
void gui_create(struct gui_window *window, unsigned int width, unsigned int height);
void gui_destroy(struct gui_window *window);

unsigned int gui_width(const struct gui_window *window);
unsigned int gui_height(const struct gui_window *window);

void gui_set_pixel(struct gui_window *window, unsigned x, unsigned y, unsigned color);
void gui_set_pixel_raw(struct gui_window *window, unsigned long i, unsigned color);
int gui_set_pixel_safe(struct gui_window *window, unsigned x, unsigned y, unsigned color);
int gui_set_pixel_raw_safe(struct gui_window *window, unsigned long i, unsigned color);
void gui_draw(const struct gui_window *window);

void gui_key_hook(struct gui_window *window, key_hook_t hook);
void gui_mouse(const struct gui_window *window, unsigned *x, unsigned *y);
void gui_mouse_raw(const struct gui_window *window, unsigned long *i);
void gui_wfi(struct gui_window *window);

#define KEY_W 119u
#define KEY_A 97u
#define KEY_S 115u
#define KEY_D 100u

#define COLOR_BLACK	0x000000
#define COLOR_WHITE	0xFFFFFF
#define COLOR_RED	0xFF0000
#define COLOR_GREEN	0x00FF00
#define COLOR_BLUE	0x0000FF

struct gui_window {
	void *__PY_window;
	unsigned int *__raw_pixels;
	unsigned int __width;
	unsigned int __height;
	unsigned long __length;
};

#endif /* GUILIB_BACKEND_H */
