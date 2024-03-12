
class GUIwindow():
    """
    A class to represent GUI window with raw pixel access and keyboard/mouse
    event management

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

    def __init__(self, width, height, winid=None):
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

    def mouse(self):
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

