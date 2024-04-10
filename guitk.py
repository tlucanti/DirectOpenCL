#!/usr/bin/python3

from py.frontend.TKwindow import TKwindow
from py.cpython import main

import sys

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('usage: ./guitk.py [path-to-executable.so]')
        sys.exit(1)

    main(TKwindow, './' + sys.argv[1])

