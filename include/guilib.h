
#ifndef GUILIB_H
#define GUILIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <guilib_internals.h>

struct gui_window;

typedef void (*key_hook_t)(struct gui_window *window, int keycode, bool pressed);

int gui_bootstrap(void);
int gui_finalize(void);

struct gui_window *gui_alloc(void);
int gui_create(struct gui_window *window, unsigned int width, unsigned int height);
int gui_destroy(struct gui_window *window);

unsigned int gui_width(struct gui_window *window);
unsigned int gui_height(struct gui_window *window);

void gui_set_pixel(struct gui_window *window, unsigned x, unsigned y, unsigned color);
void gui_set_pixel_raw(struct gui_window *window, unsigned long i, unsigned color);
int gui_set_pixel_safe(struct gui_window *window, unsigned x, unsigned y, unsigned color);
unsigned *gui_raw_pixels(struct gui_window *window);
int gui_draw(struct gui_window *window);

void gui_key_hook(struct gui_window *window, key_hook_t hook);
void gui_mouse(struct gui_window *window, int *x, int *y);
void gui_wfi(struct gui_window *window);

bool gui_pressed(struct gui_window *window, unsigned char keycode);
bool gui_closed(struct gui_window *window);

#define GUI_CLOSED 255

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

#define KEY_ESCAPE 27
#define KEY_DELETE 127
#define KEY_BACKQUOTE '`'
#define KEY_MINUS '-'
#define KEY_EQUAL '='
#define KEY_BACKSPACE 127
#define KEY_OPEN_BRACKET '['
#define KEY_CLOSE_BRACKET ']'
#define KEY_BACKSLASH '\\'
#define KEY_SEMICOLON ';'
#define KEY_QUOTE '\''
#define KEY_ENTER '\n'
#define KEY_COMA ','
#define KEY_POINT '.'
#define KEY_SLASH '/'
#define KEY_SPACE ' '

#define KEY_RIGHT 128
#define KEY_LEFT 129
#define KEY_UP 130
#define KEY_DOWN 131

#define KEY_TAB '\t'
#define KEY_CAPS 132
#define KEY_LSHIFT 133
#define KEY_LCTRL 134
#define KEY_LWIN 135
#define KEY_LALT 136
#define KEY_RSHIFT 137
#define KEY_RCTRL 138
#define KEY_RWIN 139
#define KEY_RALT 140

#define KEY_F1 141
#define KEY_F2 142
#define KEY_F3 143
#define KEY_F4 144
#define KEY_F5 145
#define KEY_F6 146
#define KEY_F7 147
#define KEY_F8 148
#define KEY_F9 149
#define KEY_F10 150
#define KEY_F11 151
#define KEY_F12 152
#define KEY_F13 153
#define KEY_F14 154
#define KEY_F15 155
#define KEY_F16 156
#define KEY_F17 157
#define KEY_F18 158
#define KEY_F19 159

#define KEY_NUM_0 160
#define KEY_NUM_1 161
#define KEY_NUM_2 162
#define KEY_NUM_3 163
#define KEY_NUM_4 164
#define KEY_NUM_5 165
#define KEY_NUM_6 166
#define KEY_NUM_7 167
#define KEY_NUM_8 168
#define KEY_NUM_9 169

#define KEY_NUM_DOWN KEY_NUM_2
#define KEY_NUM_LEFT KEY_NUM_4
#define KEY_NUM_RIGHT KEY_NUM_6
#define KEY_NUM_UP KEY_NUM_8

#define MOUSE_LEFT 0
#define MOUSE_RIGHT 1
#define MOUSE_MIDDLE 2

#define SCROLL_UP 64
#define SCROLL_DOWN 65

#define COLOR_BLACK	0x000000
#define COLOR_WHITE	0xFFFFFF
#define COLOR_RED	0xFF0000
#define COLOR_GREEN	0x00FF00
#define COLOR_BLUE	0x0000FF
#define COLOR_CYAN	0x00FFFF
#define COLOR_MAGENTA	0xFF00FF
#define COLOR_YELLOW	0xFFFF00
#define COLOR_PURPLE	COLOR_MAGENTA

#ifdef __cplusplus
}
#endif

#endif /* GUILIB_H */
