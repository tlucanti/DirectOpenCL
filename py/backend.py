
import numpy as np

from GUIwindow import GUIwindow

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


class gui(GUIwindow):
	def __init__(self, frontend):
		self.__frontend = frontend
		self.__image = np.zeros([frontend.height(), frontend.width(), 3], dtype=np.int8)

	def width(self):
		return self.__frontend.width()

	def height(self):
		return self.__frontend.height()

	def draw(self):
		self.__frontend.draw(self.__image)

	def key_hook(self, callback):
		self.__frontend.key_hook(callback)

	def mouse(self):
		return self.__frontend.mouse()

	def wfi(self):
		return self.__frontend.wfi()

	def set_pixel(self, x, y, color):
		self.__image[y][x] = color.array

	def set_pixel_safe(self, x, y, color):
		try:
			self.set_pixel(x, y, color)
		except IndexError:
			pass
