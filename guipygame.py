#!/usr/bin/python3

from py.frontend.PYGAMEwindow import PYGAMEwindow
from py.cpython import main

import sys

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('usage: ./guipygame.py [path-to-executable.so]')
        sys.exit(1)

    main(PYGAMEwindow, './' + sys.argv[1])

