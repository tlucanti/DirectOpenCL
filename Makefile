
CFLAGS = -Wall -Wextra -fdiagnostics-color=always -I include
PYFLAGS = $(CFLAGS) -fPIC
SIXFLAGS = $(CFLAGS) -O2 -I ../libsixel/include
STREAMFLAGS = $(CFLAGS) -O0 -g3 -I stream
CC = gcc
LD = gcc

include .config
CFLAGS += -D CONFIG_GUILIB_DEBUG=$(CONFIG_GUILIB_DEBUG)
CFLAGS += -D CONFIG_GUILIB_VERBOSE=$(CONFIG_GUILIB_VERBOSE)

CFLAGS += -D CONFIG_GUILIB_SIXEL_KEYBOARD_DELAY=$(CONFIG_GUILIB_SIXEL_KEYBOARD_DELAY)
CFLAGS += -D CONFIG_GUILIB_SIXEL_MAX_FPS_RELAX=$(CONFIG_GUILIB_SIXEL_MAX_FPS_RELAX)
CFLAGS += -D CONFIG_GUILIB_SIXEL_RAW_MODE=$(CONFIG_GUILIB_SIXEL_RAW_MODE)
CFLAGS += -D CONFIG_GUILIB_SIXEL_KEYBOARD_ENABLE=$(CONFIG_GUILIB_SIXEL_KEYBOARD_ENABLE)
CFLAGS += -D CONFIG_GUILIB_SIXEL_WARN_UNKNOWN_ESCAPE=$(CONFIG_GUILIB_SIXEL_WARN_UNKNOWN_ESCAPE)

all: py-build

py-build:
	$(CC) $(PYFLAGS) -c py/backend.c -o py/backend.o
	$(CC) $(PYFLAGS) -c src/stdguilib.c -o py/stdguilib.o
	$(CC) $(PYFLAGS) -c test.c -o py/test.o
	$(LD) -shared py/backend.o py/stdguilib.o py/test.o -o py/libbackend.so

py-tkinter: py-build
	cd py && python3 frontend.py

py-pil: py-build
	python3 py/cpython.py

sixlib:
	$(CC) $(SIXFLAGS) -c sixel/sixel.c -o sixel/sixel.o
	ar rcs libgui.a sixel/sixel.o

stdgui:
	$(CC) $(STREAMFLAGS) -c src/stdguilib.c -o src/stdguilib.o
	ar rcs libstdgui.a src/stdguilib.o

six: sixlib stdgui
	$(CC) $(SIXFLAGS) -L . \
		-L ../libsixel/src/.libs \
		test.c \
		-lgui -lstdgui -lsixel \
		-o guisixel

stream: stdgui
	$(CC) $(STREAMFLAGS) \
		-L . \
		stream/stream.c \
		stream/netsock.c \
		stream/encode.c \
		test.c \
		-lstdgui \
		-ljpeg \
		-o guistream
.PHONY: stream

