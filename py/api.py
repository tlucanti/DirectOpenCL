
import numpy as np
import time

from .frontend.GUIwindow import GUIwindow

class Color():
	def __init__(self, red, green, blue):
		self.__array = np.array([red, green, blue], dtype=np.int8)

	@property
	def array(self):
		return self.__array

	@property
	def red(self):
		return self.__arraay[0]

	@property
	def green(self):
		return self.__array[1]

	@property
	def blue(self):
		return self.__array[2]


COLOR_BLACK = Color(0, 0, 0)
COLOR_RED = Color(255, 0, 0)
COLOR_GREEN = Color(0, 255, 0)
COLOR_BLUE = Color(0, 0, 255)
COLOR_WHITE = Color(255, 255, 255)

## WINDOW LEVEL API

def gui_create(window: GUIwindow):
	window.__image = np.zeros([window.height(), window.width(), 3], dtype=np.int8)

def gui_width(window: GUIwindow):
	return window.width()

def gui_height(window: GUIwindow):
	return window.height()

def gui_draw(window: GUIwindow):
	window.draw(window.__image)

def gui_key_hook(window: GUIwindow, callback):
	window.key_hook(callback)

def gui_mouse(window: GUIwindow):
	return window.mouse()

def gui_wfi(window: GUIwindow):
	window.wfi()

def gui_set_pixel(window: GUIwindow, x: int, y: int, color: Color):
	window.__image[y][x] = color.array

def gui_set_pixel_safe(window, x, y, color: Color):
	try:
		gui_set_pixel(window, x, y, color)
	except IndexError:
		pass

## STDLIB API

__fps_time_prev = time.time()

def gui_draw_circle(window, x, y, radius, color):
	for yy in range(y - radius // 2, y + radius // 2):
		for xx in range(x - radius // 2, x + radius // 2):
			if (xx - x) ** 2 + (yy - y) ** 2 <= radius:
				gui_set_pixel_safe(window, xx, yy, color)

def gui_get_fps():
	global __fps_time_prev
	ret = 1 / (time.time() - __fps_time_prev)
	__fps_time_prev = time.time()
	return ret

### CONSTANTS

KEY_W = ord('w')
KEY_A = ord('a')
KEY_S = ord('s')
KEY_D = ord('d')
