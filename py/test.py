

from TKwindow import TKwindow
from backend import *
from keycodes import *
from stream.WEBwindow import WEBwindow as gui


import time

STEP = 10
dx, dy = 0, 0

class Get_fps():
	def __init__(self):
		self.t = time.time()

	def __call__(self):
		prev = self.t
		self.t = time.time()
		return 1 / (self.t - prev)

get_fps = Get_fps()


def callback(window, keycode, pressed):
	global dx, dy

	delta = STEP if pressed else -STEP
	if keycode == KEY_W:
		dy += -delta
	elif keycode == KEY_S:
		dy += delta
	elif keycode == KEY_A:
		dx += -delta
	elif keycode == KEY_D:
		dx += delta

def square(x):
	return x * x

def draw_circle(window, x, y, radius, color):
	for yy in range(y - radius // 2, y + radius // 2):
		for xx in range(x - radius // 2, x + radius // 2):
			if square(xx - x) + square(yy - y) <= radius:
				window.set_pixel_safe(xx, yy, color)

def main():
	width = 800
	height = 600
	window = gui(TKwindow(width, height))

	window.key_hook(callback)
	x, y = width // 2, height // 2

	while True:
		print('fps', get_fps())

		draw_circle(window, x, y, 40, COLOR_BLACK)
		x = (x + dx + width) % width
		y = (y + dy + height) % height
		draw_circle(window, x, y, 40, COLOR_BLUE)

		window.draw()

if __name__ == '__main__':

	width = 800
	height = 600
	window = gui(TKwindow(width, height))
