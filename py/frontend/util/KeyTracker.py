
import threading
import time
import platform
from collections import defaultdict

class _KeyTrackerLinux():

    DELTA = 0.05

    class KeyMetadata():
        def __init__(self):
            self.last_press_time = 0
            self.last_release_time = 0
            self.released = False

    def __init__(self, callback):
        self.metadata = defaultdict(self.KeyMetadata)
        self.callback = callback

    def press(self, event):
        meta = self.metadata[event.keycode]

        meta.last_press_time = time.time()
        if meta.last_press_time - meta.last_release_time > self.DELTA:
            meta.released = False
            self.callback(event.keycode, True)

    def release(self, event):
        meta = self.metadata[event.keycode]

        meta.last_release_time = time.time()
        threading.Timer(self.DELTA,
                        self._release_debounce,
                        args=[event, meta]).start()

    def _release_debounce(self, event, meta):
        if time.time() - meta.last_press_time > self.DELTA and not meta.released:
            meta.released = True
            self.callback(event.keycode, False)

class _KeyTrackerWindows():

    def __init__(self, callback):
        self.pressed = set()
        self.callback = callback

    def press(self, event):
        if event.keycode not in self.pressed:
            self.pressed.add(event.keycode)
            self.callback(event.keycode, True)

    def release(self, event):
        self.pressed.discard(event.keycode)
        self.callback(event.keycode, False)


if platform.system() == 'Windows':
    KeyTracker = _KeyTrackerWindows
else:
    KeyTracker = _KeyTrackerLinux


if __name__ == '__main__':
    from tkinter import *

    def key_callback(code, pressed):
        if pressed:
            print('pressed', code)
        else:
            print('released', code)

    window = Tk()

    key_tracker = KeyTracker(key_callback)
    window.bind('<KeyPress>', key_tracker.press)
    window.bind('<KeyRelease>', key_tracker.release)

    window.mainloop()
