

# from py.frontend.TKwindow import TKwindow as Window
from py.frontend.PYGAMEwindow import PYGAMEwindow as Window
#from py.frontend.WEBwindow import WEBwindow as Window
#from py.frontend.QTwindow import QTwindow as Window

from py.api import *

import time

STEP = 10
dx, dy = 0, 0
mouse1 = False

def test_callback(window, keycode, pressed):
    if pressed:
        print(f'key {keycode} pressed')
    else:
        print(f'key {keycode} released')

def callback(window, keycode, pressed):
    global dx, dy, mouse1

    delta = STEP if pressed else -STEP
    if keycode == KEY_W:
        dy += -delta
    elif keycode == KEY_S:
        dy += delta
    elif keycode == KEY_A:
        dx += -delta
    elif keycode == KEY_D:
        dx += delta
    elif keycode == MOUSE_LEFT:
        mouse1 = pressed

def main():
    width, height = 800, 600
    x, y = width // 2, height // 2
    mx2, my2 = 0, 0

    window = Window(width, height)
    gui_create(window)
    gui_key_hook(window, test_callback)
    #gui_key_hook(window, test_callback)

    frame = 0
    while not gui_closed(window):
        # print('\rfps\t    ', gui_get_fps(), end='')

        gui_draw_borders(window, 4, COLOR_RED)

        gui_draw_circle(window, x, y, 40, COLOR_BLACK)
        x = (x + dx + width) % width
        y = (y + dy + height) % height
        gui_draw_circle(window, x, y, 40, COLOR_BLUE)

        mx1, my1 = gui_mouse(window)
        if mouse1:
            gui_draw_line(window, mx1, my1, mx2, my2, COLOR_GREEN)
        mx2 = mx1
        my2 = my1

        gui_draw(window)
        frame += 1
        # print('sending frame', frame)

if __name__ == '__main__':
    main()
