

#from py.frontend.TKwindow import TKwindow as Window
from py.frontend.WEBwindow import WebWindow as Window
from py.api import *

import time

STEP = 10
dx, dy = 0, 0

def echo_callback(window, keycode, pressed):
	if pressed:
		print(f'key {keycode} pressed')
	else:
		print(f'key {keycode} released')

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

def main():
	width, height = 800, 600
	window = Window(width, height)
	gui_create(window)
	gui_key_hook(window, callback)

	x, y = width // 2, height // 2

	while True:
		print('fps', gui_get_fps())

		gui_draw_circle(window, x, y, 40, COLOR_BLACK)
		x = (x + dx + width) % width
		y = (y + dy + height) % height
		gui_draw_circle(window, x, y, 40, COLOR_BLUE)

		gui_draw(window)

if __name__ == '__main__':
	main()
