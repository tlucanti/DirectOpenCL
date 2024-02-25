
import tkinter
import numpy as np
from PIL import Image, ImageTk
import time
import threading

class TKwindow():
	def __init__(self, width, height):
		self._width = width
		self._height = height
		self._show_fps = False
		self._fps_prev = None

		self._done = False
		self._loop_thread = threading.Thread(target=self._init)
		self._loop_thread.start()
		self._pressed = set()
		while not self._done:
			pass

	def _init(self):
		width = self._width
		height = self._height

		self._root = tkinter.Tk()
		self._root.geometry(f'{width}x{height}')
		self._root.resizable(width=False, height=False)

		self._canvas = tkinter.Canvas(self._root, width=width, height=height,
				highlightthickness=0)
		self._canvas.place(x=0, y=0)

		self._key_hook = None
		self._root.bind('<Key>', self._on_key_press)
		self._root.bind('<KeyRelease>', self._on_key_release)
		self._root.bind('<Button>', self._on_mouse_press)
		self._root.bind('<ButtonRelease>', self._on_mouse_release)
		self._root.bind('<<Draw>>', self._draw)
		# self._root.protocol("WM_DELETE_WINDOW", self._on_close)

		self._done = True
		self._root.mainloop()

	@property
	def width(self):
		return self._width

	@property
	def height(self):
		return self._height

	def mouse(self):
		offset = self._root.geometry().split('+')[1:]
		x = self._root.winfo_pointerx() - int(offset[0]) - 9
		y = self._root.winfo_pointery() - int(offset[1]) - 30
		assert x >= 0 and y >= 0
		return x, y

	def key_hook(self, callback):
		self._key_hook = callback

	def draw(self, image):
		self._image = image
		self._root.event_generate('<<Draw>>')

	def _draw(self, event):
		image = self._image
		assert image.shape == (self._height, self._width, 3)

		img_pil = Image.fromarray(np.uint8(image))
		self._img = ImageTk.PhotoImage(img_pil)
		self._canvas.create_image(0, 0, anchor=tkinter.NW, image=self._img)

		if self._show_fps:
			if self._fps_prev is None:
				f = 0
			else:
				f = round(1 / (time.time() - self._fps_prev))
			self._canvas.create_text(25, 10, text=f'fps: {f}', fill='white', font=("Helvetica 10 bold"))
			self._fps_prev = time.time()

		self._root.update()

	def fps(self, flag):
		self._show_fps = bool(flag)

	def _loop(self):
		self._root.mainloop()

	def _on_key_press(self, event):
		if self._key_hook is not None and event.keycode not in self._pressed:
			self._pressed.add(event.keycode)
			self._key_hook(event.keycode, True)

	def _on_key_release(self, event):
		if self._key_hook is not None:
			self._pressed.discard(event.keycode)
			self._key_hook(event.keycode, False)

	def _on_mouse_press(self, event):
		if self._key_hook is not None:
			self._key_hook(event.num, True)

	def _on_mouse_release(self, event):
		if self._key_hook is not None:
			self._key_hook(event.num, False)


if __name__ == '__main__':
	import random

	def key_callback(key, pressed):
		if pressed:
			if key <= 3:
				print('pressed mouse at', win.mouse())
			else:
				print('pressed key:', key)
		else:
			print('released key', key)

	# win = TKwindow(1000, 800)
	win = TKwindow(600, 400)
	win.fps(True)
	win.key_hook(key_callback)

	while True:
		img = np.zeros([win.height, win.width, 3], dtype=np.uint8)
		img[:, :, random.randint(0, 2)] = 255
		img[1:-1, 1:-1, :] = 0

		win.draw(img)

