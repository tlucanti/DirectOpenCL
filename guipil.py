#!/usr/bin/python3

from py.frontend.PILwindow import PILwindow
from py.cpython import main

import sys

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('usage: ./guipil.py [path-to-executable.so]')
        sys.exit(1)

    main(PILwindow, './' + sys.argv[1])

