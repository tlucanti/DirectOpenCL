
from frontend.PYGAMEwindow import PYGAMEwindow as Window
from frontend.util.WebFrontend import WebFrontend
import sys

if __name__ == '__main__':
	if len(sys.argv) < 2:
		host = '127.0.0.1'
	else:
		host = sys.argv[1]

	WebFrontend(Window, host)
	# WebFrontend(Window, '51.250.6.69')
