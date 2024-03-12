
import platform
import time
import threading
from collections import defaultdict

if platform.system() == 'Windows':
    raise NotImplementedError
else:
    import sys, termios, tty

class Getch():

    def __init__(self, raw=False):
        self.fd = sys.stdin.fileno()
        self.orig = termios.tcgetattr(self.fd)
        if raw:
            tty.setraw(self.fd)
        else:
            tty.setcbreak(self.fd)

    def __del__(self):
        termios.tcsetattr(self.fd, termios.TCSAFLUSH, self.orig)

    def __call__(self):
        return sys.stdin.read(1)

getch = Getch()

class Tracker():

    DELTA = 0.2

    class KeyMetadata():
        def __init__(self):
            self.last_press_time = 0
            self.released = False

    def __init__(self, callback):
        self.metadata = defaultdict(self.KeyMetadata)
        self.callback = callback

    def press(self, keycode):
        meta = self.metadata[keycode]

        do_callback = (time.time() - meta.last_press_time > self.DELTA)
        threading.Timer(self.DELTA,
                        self._release_checker,
                        args=[keycode, meta]).start()
        meta.last_press_time = time.time()

        if do_callback:
            meta.released = False
            self.callback(keycode, True)

    def _release_checker(self, keycode, meta):
        if time.time() - meta.last_press_time > self.DELTA and not meta.released:
            meta.released = True
            self.callback(keycode, False)

if __name__ == '__main__':

    def key_callback(code, pressed):
        if pressed:
            print('pressed', code)
        else:
            print('released', code)

    key_tracker = Tracker(key_callback)

    while True:
        key = getch()
        key_tracker.press(key)
