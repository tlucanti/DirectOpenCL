
import time

import pygame
import numpy as np
import threading

from .GUIwindow import *

keycode_dict = {
    pygame.BUTTON_LEFT: MOUSE_LEFT,
    pygame.BUTTON_RIGHT: MOUSE_RIGHT,
    pygame.BUTTON_MIDDLE: MOUSE_MIDDLE,
    # MOUSE_4
    # MOUSE_5
    # MOUSE_6
    # MOUSE_7

    pygame.BUTTON_WHEELUP: SCROLL_UP,
    pygame.BUTTON_WHEELDOWN: SCROLL_DOWN,

    pygame.K_0: KEY_0,
    pygame.K_1: KEY_1,
    pygame.K_2: KEY_2,
    pygame.K_3: KEY_3,
    pygame.K_4: KEY_4,
    pygame.K_5: KEY_5,
    pygame.K_6: KEY_6,
    pygame.K_7: KEY_7,
    pygame.K_8: KEY_8,
    pygame.K_9: KEY_9,

    pygame.K_a: KEY_A,
    pygame.K_b: KEY_B,
    pygame.K_c: KEY_C,
    pygame.K_d: KEY_D,
    pygame.K_e: KEY_E,
    pygame.K_f: KEY_F,
    pygame.K_g: KEY_G,
    pygame.K_h: KEY_H,
    pygame.K_i: KEY_I,
    pygame.K_j: KEY_J,
    pygame.K_k: KEY_K,
    pygame.K_l: KEY_L,
    pygame.K_m: KEY_M,
    pygame.K_n: KEY_N,
    pygame.K_o: KEY_O,
    pygame.K_p: KEY_P,
    pygame.K_q: KEY_Q,
    pygame.K_r: KEY_R,
    pygame.K_s: KEY_S,
    pygame.K_t: KEY_T,
    pygame.K_u: KEY_U,
    pygame.K_v: KEY_V,
    pygame.K_w: KEY_W,
    pygame.K_x: KEY_X,
    pygame.K_y: KEY_Y,
    pygame.K_z: KEY_Z,

    pygame.K_ESCAPE: KEY_ESCAPE,
    pygame.K_DELETE: KEY_DELETE,
    pygame.K_BACKQUOTE: KEY_BACKQUOTE,
    pygame.K_MINUS: KEY_MINUS,
    pygame.K_EQUALS: KEY_EQUAL,
    pygame.K_BACKSPACE: KEY_BACKSPACE,
    pygame.K_LEFTBRACKET: KEY_LEFTBRACE,
    pygame.K_RIGHTBRACKET: KEY_RIGHTBRACE,
    pygame.K_BACKSLASH: KEY_BACKSLASH,
    pygame.K_SEMICOLON: KEY_SEMICOLON,
    pygame.K_QUOTE: KEY_QUOTE,
    pygame.K_KP_ENTER: KEY_ENTER,
    pygame.K_COMMA: KEY_COMA,
    pygame.K_PERIOD: KEY_POINT,
    pygame.K_SLASH: KEY_SLASH,
    pygame.K_SPACE: KEY_SPACE,

    pygame.K_RIGHT: KEY_RIGHT,
    pygame.K_LEFT: KEY_LEFT,
    pygame.K_UP: KEY_UP,
    pygame.K_DOWN: KEY_DOWN,

    pygame.K_TAB: KEY_TAB,
    pygame.K_CAPSLOCK: KEY_CAPS,
    pygame.K_LSHIFT: KEY_LSHIFT,
    pygame.K_LCTRL: KEY_LCTRL,
    pygame.K_LSUPER: KEY_LSUPER,
    pygame.K_LALT: KEY_LALT,
    pygame.K_RSHIFT: KEY_RSHIFT,
    pygame.K_RCTRL: KEY_RCTRL,
    pygame.K_RALT: KEY_RALT,

    pygame.K_F1: KEY_F1,
    pygame.K_F2: KEY_F2,
    pygame.K_F3: KEY_F3,
    pygame.K_F4: KEY_F4,
    pygame.K_F5: KEY_F5,
    pygame.K_F6: KEY_F6,
    pygame.K_F7: KEY_F7,
    pygame.K_F8: KEY_F8,
    pygame.K_F9: KEY_F9,
    pygame.K_F10: KEY_F10,
    pygame.K_F11: KEY_F11,
    pygame.K_F12: KEY_F12,
    pygame.K_F13: KEY_F13,
    pygame.K_F14: KEY_F14,
    pygame.K_F15: KEY_F15,
    # KEY_F16
    # KEY_F17
    # KEY_F18
    # KEY_F19

    # KEY_NUM_0
    # KEY_NUM_1
    # KEY_NUM_2
    # KEY_NUM_3
    # KEY_NUM_4
    # KEY_NUM_5
    # KEY_NUM_6
    # KEY_NUM_7
    # KEY_NUM_8
    # KEY_NUM_9
}

keycode_dict[1105] = KEY_BACKQUOTE
keycode_dict[13] = KEY_ENTER
keycode_dict[10] = KEY_ENTER

class PYGAMEwindow(GUIwindow):
    def __init__(self, width, height, winid=None):
        if winid is None:
            winid = self
        self.__winid = winid
        self.__callback = None

        self.__loop_thread = threading.Thread(target=self.__main_loop, args=[width, height])
        self.__loop_thread.start()

        self.__started = False
        while not self.__started:
            time.sleep(1e-5)

    def width(self):
        return pygame.display.get_surface().get_size()[0]

    def height(self):
        return pygame.display.get_surface().get_size()[1]

    def mouse(self):
        return pygame.mouse.get_pos()

    def key_hook(self, callback):
        self.__callback = callback

    def draw(self, image):
        #assert image.shape == (self.height(), self.width(), 3)

        pygame.surfarray.pixels3d(self.__screen)[:] = image.transpose(1, 0, 2)
        pygame.display.flip()

    def __main_loop(self, width, height):
        pygame.init()
        pygame.display.set_caption("pygame")

        self.__screen = pygame.display.set_mode((width, height))
        self.__started = True

        print('start thread')
        while True:
            event = pygame.event.wait()
            if event.type == pygame.MOUSEMOTION:
                continue

            if event.type == pygame.QUIT:
                break
            elif event.type == pygame.KEYDOWN:
                key = keycode_dict[event.key]
                pressed = True
            elif event.type == pygame.KEYUP:
                key = keycode_dict[event.key]
                pressed = False
            elif event.type == pygame.MOUSEBUTTONDOWN:
                key = keycode_dict[event.button]
                pressed = True
            elif event.type == pygame.MOUSEBUTTONUP:
                if event.button in (pygame.BUTTON_WHEELUP, pygame.BUTTON_WHEELDOWN):
                    continue
                key = keycode_dict[event.button]
                pressed = False
            else:
                continue

            print(key, pressed)
            if self.__callback is not None:
                self.__callback(self.__winid, key, pressed)
