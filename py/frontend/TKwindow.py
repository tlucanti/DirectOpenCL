
import time
import threading

import tkinter
import numpy as np
from PIL import Image, ImageTk

from .util.KeyTracker import KeyTracker
from .GUIwindow import *

keycode_dict = {
    1: MOUSE_LEFT,
    3: MOUSE_RIGHT,
    2: MOUSE_MIDDLE,

    -1: SCROLL_UP,
    -1: SCROLL_DOWN,

    ord('0'): KEY_0,
    ord('1'): KEY_1,
    ord('2'): KEY_2,
    ord('3'): KEY_3,
    ord('4'): KEY_4,
    ord('5'): KEY_5,
    ord('6'): KEY_6,
    ord('7'): KEY_7,
    ord('8'): KEY_8,
    ord('9'): KEY_9,

    ord('A'): KEY_A,
    ord('B'): KEY_B,
    ord('C'): KEY_C,
    ord('D'): KEY_D,
    ord('E'): KEY_E,
    ord('F'): KEY_F,
    ord('G'): KEY_G,
    ord('H'): KEY_H,
    ord('I'): KEY_I,
    ord('J'): KEY_J,
    ord('K'): KEY_K,
    ord('L'): KEY_L,
    ord('M'): KEY_M,
    ord('N'): KEY_N,
    ord('O'): KEY_O,
    ord('P'): KEY_P,
    ord('Q'): KEY_Q,
    ord('R'): KEY_R,
    ord('S'): KEY_S,
    ord('T'): KEY_T,
    ord('U'): KEY_U,
    ord('V'): KEY_V,
    ord('W'): KEY_W,
    ord('X'): KEY_X,
    ord('Y'): KEY_Y,
    ord('Z'): KEY_Z,

     27: KEY_ESCAPE,
     46: KEY_DELETE,
    192: KEY_BACKQUOTE,
    189: KEY_MINUS,
    187: KEY_EQUAL,
      8: KEY_BACKSPACE,
    219: KEY_LEFTBRACE,
    221: KEY_RIGHTBRACE,
    220: KEY_BACKSLASH,
    186: KEY_SEMICOLON,
    222: KEY_QUOTE,
     13: KEY_ENTER,
    188: KEY_COMA,
    190: KEY_POINT,
    191: KEY_SLASH,
     32: KEY_SPACE,

     39: KEY_RIGHT,
     37: KEY_LEFT,
     38: KEY_UP,
     40: KEY_DOWN,

      9: KEY_TAB,
     -1: KEY_CAPS,
     16: KEY_LSHIFT,
     17: KEY_LCTRL,
     91: KEY_LSUPER,
     18: KEY_LALT,
     16: KEY_RSHIFT,
     17: KEY_RCTRL,
     18: KEY_RALT,
    255: KEY_FN,

    112  : KEY_F1,
    113: KEY_F2,
    114: KEY_F3,
    115: KEY_F4,
    116: KEY_F5,
    117: KEY_F6,
    118: KEY_F7,
    119: KEY_F8,
    120: KEY_F9,
    121: KEY_F10,
    122: KEY_F11,
    123: KEY_F12,
    124: KEY_F13,
    125: KEY_F14,
    126: KEY_F15,
    -1: KEY_F16,
    -1: KEY_F17,
    -1: KEY_F18,
    -1: KEY_F19,

    -1: KEY_NUM_0,
    -1: KEY_NUM_1,
    -1: KEY_NUM_2,
    -1: KEY_NUM_3,
    -1: KEY_NUM_4,
    -1: KEY_NUM_5,
    -1: KEY_NUM_6,
    -1: KEY_NUM_7,
    -1: KEY_NUM_8,
    -1: KEY_NUM_9,
}

class TKwindow(GUIwindow):
    def __init__(self, width, height, winid=None):
        self.__width = width
        self.__height = height
        self.__winid = self if winid is None else winid
        self.__show_fps = False
        self.__fps_prev = 0
        self.__key_callback = None

        self.__done = False
        self.__loop_thread = threading.Thread(target=self.__init_window)
        self.__loop_thread.start()
        while not self.__done:
            time.sleep(1e-5)

    def __init_window(self):
        self.__root = tkinter.Tk()
        self.__root.geometry(f'{self.__width}x{self.__height}')
        self.__root.resizable(width=False, height=False)

        self.__canvas = tkinter.Canvas(self.__root, width=self.__width,
                                       height=self.__height, highlightthickness=0)
        self.__canvas.place(x=0, y=0)

        self.__key_tracker = KeyTracker(self.__key_handler)
        self.__root.bind('<Key>', self.__key_tracker.press)
        self.__root.bind('<KeyRelease>', self.__key_tracker.release)
        self.__root.bind('<Button>', self.__on_mouse_press)
        self.__root.bind('<ButtonRelease>', self.__on_mouse_release)
        self.__root.bind('<MouseWheel>', self.__on_mouse_wheel)
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

    def draw(self, image):
        assert image.shape == (self.__height, self.__width, 3)

        self.__image = image
        self.__done = False
        self.__root.event_generate('<<Draw>>')
        while not self.__done:
            time.sleep(1e-5)

    def __do_draw(self, event):
        img = Image.fromarray(np.uint8(self.__image))

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

    def __key_handler(self, keycode, pressed):
        if self.__key_callback is not None:
            keycode = keycode_dict.get(keycode, keycode)
            self.__key_callback(self.__winid, keycode, pressed)

    def __on_mouse_press(self, event):
        if self.__key_callback is not None:
            keycode = keycode_dict.get(event.num, event.num)
            self.__key_callback(self.__winid, keycode, True)

    def __on_mouse_release(self, event):
        if self.__key_callback is not None:
            keycode = keycode_dict.get(event.num, event.num)
            self.__key_callback(self.__winid, keycode, False)

    def __on_mouse_wheel(self, event):
        if event.delta >= 0:
            keycode = SCROLL_UP
        else:
            keycode = SCROLL_DOWN
        if self.__key_callback:
            self.__key_callback(self.__winid, keycode, True)


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
