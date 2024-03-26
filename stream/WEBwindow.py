
import stream.client.Client as Client

class WEBwindow():
    def __init__(self, frontend):
		self.__frontend = frontend
		self.__image = np.zeros([frontend.height(), frontend.width(), 3], dtype=np.int8)
        self.__client = Client()
        self.__frontend.key_hook(self.__key_callback)

    def __key_callback(self, keycode, pressed):
        self.__client.send(f'keyboard event {keycode} {pressed}')


