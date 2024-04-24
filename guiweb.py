#!/usr/bin/python3

from py.frontend.WEBwindow import WEBwindow
from py.cpython import main

import sys

if __name__ == '__main__':
	if len(sys.argv) != 2:
		print('usage: ./guiweb.py [path-to-executable.so]')
		sys.exit(1)

	main(WEBwindow, './' + sys.argv[1])

