
import ctypes
import numpy as np

#from TKwindow import TKwindow as Window
from frontend.PILwindow import PILwindow as Window

@ctypes.CFUNCTYPE(ctypes.py_object, ctypes.c_uint, ctypes.c_uint, ctypes.c_void_p)
def window_construct(width, height, winptr):
    window = Window(width, height, winptr)
    return window

@ctypes.CFUNCTYPE(None, ctypes.py_object)
def window_destroy(window):
    del window

@ctypes.CFUNCTYPE(None, ctypes.py_object, ctypes.POINTER(ctypes.c_uint))
def window_draw(window, uptr):
    raw_pixels = np.ctypeslib.as_array(uptr, shape=[window.width() * window.height()])
    pixels = raw_pixels.copy().reshape(window.height(), window.width())
    red = (pixels & 0xFF0000) >> 16
    green = (pixels & 0x00FF00) >> 8
    blue = pixels & 0x0000FF
    image = np.stack([red, green, blue], axis=2)

    assert image.shape == (window.height(), window.width(), 3)
    window.draw(image)

key_hook_t = ctypes.CFUNCTYPE(None, ctypes.py_object, ctypes.c_int, ctypes.c_bool)
@ctypes.CFUNCTYPE(None, ctypes.py_object, key_hook_t)
def window_key_hook(window, callback):
    window.key_hook(callback)

@ctypes.CFUNCTYPE(None, ctypes.py_object)
def window_wfi(window):
    window.wfi()

@ctypes.CFUNCTYPE(None, ctypes.py_object, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int))
def window_mouse(window, x_ptr, y_ptr):
    x, y = window.mouse()
    x_ptr[0] = x
    y_ptr[0] = y


def main():
    dll = ctypes.cdll.LoadLibrary('py/libbackend.so')

    dll._set_PY_window_constructor(window_construct)
    dll._set_PY_window_destructor(window_destroy)
    dll._set_PY_window_draw(window_draw)
    dll._set_PY_window_key_hook(window_key_hook)
    dll._set_PY_window_wfi(window_wfi)
    dll._set_PY_window_mouse(window_mouse)

    dll.main()
    print('py: main done')

if __name__ == '__main__':
    main()
