
import tkinter
import numpy as np
from PIL import Image, ImageTk
import time

class TKwindow():
	def __init__(self, width, height):
		self._width = width
		self._height = height

		self._root = tkinter.Tk()
		self._root.geometry(f'{width}x{height}')
		self._root.resizable(width=False, height=False)

		self._canvas = tkinter.Canvas(self._root, width=width, height=height,
				highlightthickness=0)
		self._canvas.place(x=0, y=0)

	@property
	def width(self):
		return self._width

	@property
	def height(self):
		return self._height

	def draw(self, image, x=0, y=0):
		assert image.shape == (self._height, self._width, 3)

		img_pil = Image.fromarray(np.uint8(image))
		self._img = ImageTk.PhotoImage(img_pil)
		self._canvas.create_image(x, y, anchor=tkinter.NW, image=self._img)

		self._root.update()

	def fps(self, flag):
		self._show_fps = bool(flag)

	def loop(self):
		self._root.mainloop()


if __name__ == '__main__':
	import random

	win = TKwindow(1000, 800)
	prev = time.time()
	while True:
		img = np.zeros([win.height, win.width, 3], dtype=np.uint8)
		img[:, :, random.randint(0, 2)] = 255
		img[1:-1, 1:-1, :] = 0
		win.draw(img)

		t = time.time()
		print('fps:', 1 / (t - prev))
		prev = t

