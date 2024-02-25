

import tkinter
import numpy as np
from PIL import Image, ImageTk
import random

IM_WIDTH = 600
IM_HEIGHT = 400

root = tkinter.Tk()
root.geometry(f'{IM_WIDTH}x{IM_HEIGHT}')

canvas = tkinter.Canvas(root, width=IM_WIDTH, height=IM_HEIGHT, highlightthickness=0)
canvas.place(x=0, y=0)

while True:
	img_arr = np.zeros([IM_HEIGHT, IM_WIDTH, 3], dtype=np.uint8)
	img_arr[:,:,0] = random.randint(0, 255)
	img_arr[1:-1, 1:-1, 0] = 0

	img_pil = Image.fromarray(np.uint8(img_arr))
	img_tk = ImageTk.PhotoImage(img_pil)
	canvas.create_image(0, 0, anchor=tkinter.NW, image=img_tk)

	root.update()

