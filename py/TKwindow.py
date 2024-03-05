
import time
import threading

import tkinter
import numpy as np
from PIL import Image, ImageTk

from KeyTracker import KeyTracker
from GUIwindow import GUIwindow

class TKwindow(GUIwindow):
    def __init__(self, width, height):
        self.__width = width
        self.__height = height
        self.__show_fps = False
        self.__fps_prev = 0

        self.__done = False
        self.__loop_thread = threading.Thread(target=self.__init_window)
        self.__loop_thread.start()
        self.__pressed = set()
        while not self.__done:
            time.sleep(0.1)

    def __init_window(self):
        self.__root = tkinter.Tk()
        self.__root.geometry(f'{self.__width}x{self.__height}')
        self.__root.resizable(width=False, height=False)

        self.__canvas = tkinter.Canvas(self.__root, width=self.__width,
                                       height=self.__height, highlightthickness=0)
        self.__canvas.place(x=0, y=0)

        self.__key_tracker = KeyTracker(None)
        self.__root.bind('<Key>', self.__key_tracker.press)
        self.__root.bind('<KeyRelease>', self.__key_tracker.release)
        self.__root.bind('<Button>', self.__on_mouse_press)
        self.__root.bind('<ButtonRelease>', self.__on_mouse_release)
        self.__root.bind('<<Draw>>', self.__do_draw)
        self.last = 0

        self.__im_id = 0
        self.__txt_id = 0
        self.__done = True
        self.__root.mainloop()

    def width(self):
        return self.__width

    def height(self):
        return self.__height

    def mouse(self):
        offset = self.__root.geometry().split('+')[1:]
        x = self.__root.winfo_pointerx() - int(offset[0]) - 9
        y = self.__root.winfo_pointery() - int(offset[1]) - 30
        return x, y

    def key_hook(self, callback):
        self.__key_callback = callback
        self.__key_tracker.callback = callback

    def draw(self, image):
        assert image.shape == (self.__height, self.__width, 3)

        self.__image = image
        self.__done = False
        self.__root.event_generate('<<Draw>>')
        while not self.__done:
            time.sleep(1e-5)

    def __do_draw(self, event):
        img = Image.fromarray(np.uint8(self.__image))

        self.__canvas.delete('img')

        self.__tk_img = ImageTk.PhotoImage(img)

        self.__canvas.delete(self.__im_id)
        self.__im_id = self.__canvas.create_image(0, 0,
                              anchor=tkinter.NW,
                              image=self.__tk_img)

        if self.__show_fps:
            f = round(1 / (time.time() - self.__fps_prev))

            self.__canvas.delete(self.__txt_id)
            self.__txt_id = self.__canvas.create_text(25, 10, text=f'fps: {f}',
                                                      fill='white',
                                                      font=("Helvetica 10 bold"))
            self.__fps_prev = time.time()
        self.__root.update()
        self.__done = True

    def fps(self, flag):
        self.__show_fps = bool(flag)

    def __on_mouse_press(self, event):
        if self.__key_callback is not None:
            self.__key_callback(event.num, True)

    def __on_mouse_release(self, event):
        if self.__key_callback is not None:
            self.__key_callback(event.num, False)


if __name__ == '__main__':
    import random

    def key_callback(key, pressed):
        t = 'pressed' if pressed else 'released'

        if key <= 2:
            print(f'{t} mouse key {key} at {win.mouse()}')
        else:
            print(f'{t} keyboard key {key}')

    win = TKwindow(600, 400)
    win.fps(True)
    win.key_hook(key_callback)

    img = np.zeros([win.height(), win.width(), 3], dtype=np.uint8)
    while True:
        img.fill(0)
        img[:, :, random.randint(0, 2)] = 255
        img[1:-1, 1:-1, :] = 0

        win.draw(img)
