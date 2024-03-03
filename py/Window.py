
class Window():
    """
    A class to represent GUI window with raw pixel access and keyboard/mouse
    event management

    Methods
    -------

    Constructor(width, height)
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
    """

    def __init__(self, width, height):
        """
        Constructor reates new gui window with given window size

        Parameters
        ----------
        width : positive int
            the width of creating window in pixels
        height : positive int
            the height of creating window in pixels

        """
        pass

    def width(self):
        """
        Get window width

        Returns
        -------
        positive int
            horizontal size of avaliable draw area of the window in pixels
        """
        pass

    def height(self):
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
            function should take 2 arguments:
             - positive integer: keycode of pressed or released keyboard key or
               mouse button
             - bool: flag of event type (True - key was pressed,
               False - released)
            function will be called on each event with corresponding arguments
        """
        pass

    def mouse(self):
        """
        Get current position of mouse pointer
        """
        pass

