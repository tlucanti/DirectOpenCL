
#ifndef GUILIB_BACKEND_H
#define GUILIB_BACKEND_H

#include <stdbool.h>

struct gui_window;

typedef void (*key_hook_t)(struct gui_window *window, int keycode, bool pressed);

void gui_bootstrap(void);
void gui_finalize(void);
void gui_create(struct gui_window *window, unsigned int width, unsigned int height);
void gui_destroy(struct gui_window *window);

unsigned int gui_width(struct gui_window *window);
unsigned int gui_height(struct gui_window *window);

void gui_set_pixel(struct gui_window *window, unsigned x, unsigned y, unsigned color);
void gui_set_pixel_raw(struct gui_window *window, unsigned long i, unsigned color);
int gui_set_pixel_safe(struct gui_window *window, unsigned x, unsigned y, unsigned color);
unsigned *gui_raw_pixels(struct gui_window *window);
void gui_draw(struct gui_window *window);

void gui_key_hook(struct gui_window *window, key_hook_t hook);
void gui_mouse(struct gui_window *window, int *x, int *y);
void gui_wfi(struct gui_window *window);

#define KEY_0 '0'
#define KEY_1 '1'
#define KEY_2 '2'
#define KEY_3 '3'
#define KEY_4 '4'
#define KEY_5 '5'
#define KEY_6 '6'
#define KEY_7 '7'
#define KEY_8 '8'
#define KEY_9 '9'

#define KEY_A 'a'
#define KEY_B 'b'
#define KEY_C 'c'
#define KEY_D 'd'
#define KEY_E 'e'
#define KEY_F 'f'
#define KEY_G 'g'
#define KEY_H 'h'
#define KEY_I 'i'
#define KEY_J 'j'
#define KEY_K 'k'
#define KEY_L 'l'
#define KEY_M 'm'
#define KEY_N 'n'
#define KEY_O 'o'
#define KEY_P 'p'
#define KEY_Q 'q'
#define KEY_R 'r'
#define KEY_S 's'
#define KEY_T 't'
#define KEY_U 'u'
#define KEY_V 'v'
#define KEY_W 'w'
#define KEY_X 'x'
#define KEY_Y 'y'
#define KEY_Z 'z'

#define KEY_MINUS '-'
#define KEY_PLUS '+'
#define KEY_SPACE ' '
#define KEY_TILDA '~'
#define KEY_EQUAL '='
#define KEY_QUOTE '`'
#define KEY_SINGLE_QUOTE '\''
#define KEY_SEMICOLON ';'
#define KEY_OPEN_BRACKET '['
#define KEY_CLOSE_BRACKET ']'
#define KEY_BACKSLASH '\\'
#define KEY_TAB '\t'

#define KEY_NUM_0 0
#define KEY_NUM_1 0
#define KEY_NUM_2 0
#define KEY_NUM_3 0
#define KEY_NUM_4 0
#define KEY_NUM_5 0
#define KEY_NUM_6 0
#define KEY_NUM_7 0
#define KEY_NUM_8 0
#define KEY_NUM_9 0

#define KEY_NUM_RIGHT KEY_NUM_6
#define KEY_NUM_LEFT KEY_NUM_4
#define KEY_NUM_UP KEY_NUM_8
#define KEY_NUM_DOWN KEY_NUM_2

#define KEY_BACKSPACE '\x7f'
#define KEY_ENTER '\n'
#define KEY_ESCAPE 0

#define KEY_RIGHT 128
#define KEY_LEFT 129
#define KEY_UP 130
#define KEY_DOWN 131

#define MOUSE_LEFT 0
#define MOUSE_RIGHT 1
#define MOUSE_MIDDLE 2
#define MOUSE_SCROLL_UP 64
#define MOUSE_SCROLL_DOWN 65

#define COLOR_BLACK	0x000000
#define COLOR_WHITE	0xFFFFFF
#define COLOR_RED	0xFF0000
#define COLOR_GREEN	0x00FF00
#define COLOR_BLUE	0x0000FF
#define COLOR_CYAN	0x00FFFF
#define COLOR_MAGENTA	0xFF00FF
#define COLOR_YELLOW	0xFFFF00
#define COLOR_PURPLE	COLOR_MAGENTA

#if 1
#include <pthread.h>
#include <netsock.h>
struct gui_window {
        unsigned *__raw_pixels;
        unsigned char *__compressed;
        struct soc_stream event_socket;
        struct soc_stream pix_socket;
        unsigned int __width;
        unsigned int __height;
        unsigned int __length;
        int __mouse_x;
        int __mouse_y;
        key_hook_t __callback;
        pthread_t __key_thread;
        bool __waiting_for_mouse;
        bool __waiting_for_interrupt;
        bool __key_reader_run;
};
#elif 0
struct gui_window {
	void *__PY_window;
	unsigned int *__raw_pixels;
	unsigned int __width;
	unsigned int __height;
	unsigned long __length;
};
#else
#include <sixel.h>

struct gui_window {
	unsigned int __width;
	unsigned int __height;
	unsigned int __length;
	key_hook_t __callback;
	int __mouse_x;
	int __mouse_y;
	unsigned int *__raw_pixels;
	sixel_output_t *__output;
	sixel_dither_t *__dither;
};
#endif

#endif /* GUILIB_BACKEND_H */
