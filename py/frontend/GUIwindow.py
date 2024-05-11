
class GUIwindow():
    """
    A base class to represent GUI window with raw pixel access and
    keyboard/mouse event management

    Methods
    -------

    Constructor(width, height, winid)
        creates new window with given geometry

    width():
        returns draw area width in pixels

    height():
        returns draw area height in pixels

    draw(image):
        draws raw pixels from numpy array with rgb colors

    key_hook(callback):
        set callback for asynchronous call when key pressed

    mouse():
        returns current mouse position

    wfi():
        wait for interrupt
    """

    def __init__(self, width: int, height: int, winid=None):
        """
        Constructor reates new gui window with given window size

        Parameters
        ----------
        width : positive int
            the width of creating window in pixels
        height : positive int
            the height of creating window in pixels
        winptr : any, optional
            the window identification data. This value will be sent as first
            argument to keyboard callback function to identify from what window
            was keyboard interrupt raised. If winid is None - self (this object)
            will be used instead
        """
        pass

    def width(self) -> int:
        """
        Get window width

        Returns
        -------
        positive int
            horizontal size of avaliable draw area of the window in pixels
        """
        pass

    def height(self) -> int:
        """
        Get window height

        Returns
        -------
            vertical size of avaliable draw area of the window in pixels
        """
        pass

    def draw(self, image):
        """
        Update window contents

        Parameters
        ----------
        image : array
            array of positive integers in range from 0 to 255 with length of
            size() * width(). Window contents will be updated using pixel color
            data from this array. Pixel with coordinates (x, y) will take color
            from array at index (y * width + x)
        """
        pass

    def key_hook(self, callback):
        """
        Set hook to key/mouse press events

        Parameters
        ----------
        callback : function
            function should take 3 arguments:
             - any: window identificator which was passed into constructor of
               this class
             - positive integer: keycode of pressed or released keyboard key or
               mouse button
             - bool: flag of event type (True - key was pressed,
               False - released)
            function will be called on each event with corresponding arguments
        """
        pass

    def mouse(self) -> tuple:
        """
        Get current position of mouse pointer

        Returns
        -------
        tuple:
            pair of integers - horizontal and vertical coordinates of current
            mouse position. Coordinates has position relative to the top left
            window border. Note, that coordinates can have negative values or
            values that are larger then width() or height() if mouse if beyond
            the window at the monent of function call
        """
        pass

    def wfi(self):
        """
        Wait for any keyboard event. Function blocks until any key is pressed,
        even if keyboard callback is not set

        Returns
        -------
        None
        """
        pass

    def pressed(self, key: int) -> bool:
        """
        Get status of keyboard key

        Returns
        -------
        True if @key is pressed otherwise False
        """

    def closed(self) -> bool:
        """
        Get status of the window

        Returns
        -------
        True if window was closed otherwise False
        """


GUI_CLOSED      = 255

MOUSE_LEFT      = 0
MOUSE_RIGHT     = 1
MOUSE_MIDDLE    = 2
MOUSE_4         = 4
MOUSE_5         = 5
MOUSE_6         = 6
MOUSE_7         = 7

SCROLL_UP   = 64
SCROLL_DOWN = 65

KEY_0 = ord('0')
KEY_1 = ord('1')
KEY_2 = ord('2')
KEY_3 = ord('3')
KEY_4 = ord('4')
KEY_5 = ord('5')
KEY_6 = ord('6')
KEY_7 = ord('7')
KEY_8 = ord('8')
KEY_9 = ord('9')

KEY_A = ord('a')
KEY_B = ord('b')
KEY_C = ord('c')
KEY_D = ord('d')
KEY_E = ord('e')
KEY_F = ord('f')
KEY_G = ord('g')
KEY_H = ord('h')
KEY_I = ord('i')
KEY_J = ord('j')
KEY_K = ord('k')
KEY_L = ord('l')
KEY_M = ord('m')
KEY_N = ord('n')
KEY_O = ord('o')
KEY_P = ord('p')
KEY_Q = ord('q')
KEY_R = ord('r')
KEY_S = ord('s')
KEY_T = ord('t')
KEY_U = ord('u')
KEY_V = ord('v')
KEY_W = ord('w')
KEY_X = ord('x')
KEY_Y = ord('y')
KEY_Z = ord('z')

KEY_ESCAPE      = 27
KEY_DELETE      = 127
KEY_BACKQUOTE   = ord('`')
KEY_MINUS       = ord('-')
KEY_EQUAL       = ord('=')
KEY_BACKSPACE   = 8
KEY_LEFTBRACE   = ord('[')
KEY_RIGHTBRACE  = ord(']')
KEY_BACKSLASH   = ord('\\')
KEY_SEMICOLON   = ord(';')
KEY_QUOTE       = ord('\'')
KEY_ENTER       = ord('\n')
KEY_COMA        = ord(',')
KEY_POINT       = ord('.')
KEY_SLASH       = ord('/')
KEY_SPACE       = ord(' ')

KEY_RIGHT = 128
KEY_LEFT = 129
KEY_UP = 130
KEY_DOWN = 131

KEY_TAB = ord('\t')
KEY_CAPS = 132
KEY_LSHIFT = 133
KEY_LCTRL = 134
KEY_FN = 135
KEY_LSUPER = 136
KEY_LALT = 137
KEY_RSHIFT = 138
KEY_RCTRL = 139
KEY_RSUPER = 149
KEY_RALT = 141

KEY_F1 = 142
KEY_F2 = 143
KEY_F3 = 144
KEY_F4 = 145
KEY_F5 = 146
KEY_F6 = 147
KEY_F7 = 148
KEY_F8 = 149
KEY_F9 = 159
KEY_F10 = 151
KEY_F11 = 152
KEY_F12 = 153
KEY_F13 = 154
KEY_F14 = 155
KEY_F15 = 156
KEY_F16 = 157
KEY_F17 = 158
KEY_F18 = 159
KEY_F19 = 160

KEY_NUM_0 = 161
KEY_NUM_1 = 162
KEY_NUM_2 = 163
KEY_NUM_3 = 164
KEY_NUM_4 = 165
KEY_NUM_5 = 166
KEY_NUM_6 = 167
KEY_NUM_7 = 168
KEY_NUM_8 = 169
KEY_NUM_9 = 170

KEY_NUM_DOWN = KEY_NUM_2
KEY_NUM_LEFT = KEY_NUM_4
KEY_NUM_RIGHT = KEY_NUM_6
KEY_NUM_UP = KEY_NUM_8

KEY_NAMES = {
    GUI_CLOSED: 'WINDOW CLOSE EVENT',

    MOUSE_LEFT: 'MOUSE LEFT',
    MOUSE_RIGHT: 'MOUSE RIGHT',
    MOUSE_MIDDLE: 'MOUSE MIDDLE',
    MOUSE_4: 'MOUSE 4',
    MOUSE_5: 'MOUSE 5',
    MOUSE_6: 'MOUSE 6',
    MOUSE_7: 'MOUSE 7',

    SCROLL_UP: 'SCROLL UP',
    SCROLL_DOWN: 'SCROLL DOWN',

    KEY_0: '0',
    KEY_1: '1',
    KEY_2: '2',
    KEY_3: '3',
    KEY_4: '4',
    KEY_5: '5',
    KEY_6: '6',
    KEY_7: '7',
    KEY_8: '8',
    KEY_9: '9',

    KEY_A: 'a',
    KEY_B: 'b',
    KEY_C: 'c',
    KEY_D: 'd',
    KEY_E: 'e',
    KEY_F: 'f',
    KEY_G: 'g',
    KEY_H: 'h',
    KEY_I: 'i',
    KEY_J: 'j',
    KEY_K: 'k',
    KEY_L: 'l',
    KEY_M: 'm',
    KEY_N: 'n',
    KEY_O: 'o',
    KEY_P: 'p',
    KEY_Q: 'q',
    KEY_R: 'r',
    KEY_S: 's',
    KEY_T: 't',
    KEY_U: 'u',
    KEY_V: 'v',
    KEY_W: 'w',
    KEY_X: 'x',
    KEY_Y: 'y',
    KEY_Z: 'z',

    KEY_ESCAPE: 'ESC',
    KEY_DELETE: 'DEL',
    KEY_BACKQUOTE: '`',
    KEY_MINUS: '-',
    KEY_EQUAL: '=',
    KEY_BACKSPACE: 'BACKSPACE',
    KEY_LEFTBRACE: '[',
    KEY_RIGHTBRACE: ']',
    KEY_BACKSLASH: '\\',
    KEY_SEMICOLON: ';',
    KEY_QUOTE: '\'',
    KEY_COMA: ',',
    KEY_POINT: '.',
    KEY_SLASH: '/',
    KEY_SPACE: 'SPACE',

    KEY_RIGHT: 'RIGHT',
    KEY_LEFT: 'LEFT',
    KEY_UP: 'UP',
    KEY_DOWN: 'DOWN',

    KEY_TAB: 'TAB',
    KEY_CAPS: 'CAPS',
    KEY_LSHIFT: 'LSHIFT',
    KEY_LCTRL: 'LCTRL',
    KEY_FN: 'FN',
    KEY_LSUPER: 'LSUPER',
    KEY_LALT: 'LALT',
    KEY_RSHIFT: 'RSHIFT',
    KEY_RCTRL: 'RCTRL',
    KEY_RSUPER: 'RSUPER',
    KEY_RALT: 'RALT',

    KEY_F1: 'F1',
    KEY_F2: 'F2',
    KEY_F3: 'F3',
    KEY_F4: 'F4',
    KEY_F5: 'F5',
    KEY_F6: 'F6',
    KEY_F7: 'F7',
    KEY_F8: 'F8',
    KEY_F9: 'F9',
    KEY_F10: 'F10',
    KEY_F11: 'F11',
    KEY_F12: 'F12',
    KEY_F13: 'F13',
    KEY_F14: 'F14',
    KEY_F15: 'F15',
    KEY_F16: 'F16',
    KEY_F17: 'F17',
    KEY_F18: 'F18',
    KEY_F19: 'F19',
}
