
CFLAGS = -Wall -Wextra -fdiagnostics-color=always -I include
PYFLAGS = $(CFLAGS) -fPIC
SIXFLAGS = $(CFLAGS) -O2 -I ../libsixel/include
STREAMFLAGS = $(CFLAGS) -O0 -g3 -I stream
CC = gcc
LD = gcc

include .config
CFLAGS += -D CONFIG_GUILIB_DEBUG=$(CONFIG_GUILIB_DEBUG)
CFLAGS += -D CONFIG_GUILIB_VERBOSE=$(CONFIG_GUILIB_VERBOSE)

CFLAGS += -D CONFIG_GUILIB_STREAM_COMPRESSION_TYPE=$(CONFIG_GUILIB_STREAM_COMPRESSION_TYPE)
CFLAGS += -D CONFIG_GUILIB_STREAM_COMPRESSION_QUALITY=$(CONFIG_GUILIB_STREAM_COMPRESSION_QUALITY)

CFLAGS += -D CONFIG_GUILIB_SIXEL_KEYBOARD_DELAY=$(CONFIG_GUILIB_SIXEL_KEYBOARD_DELAY)
CFLAGS += -D CONFIG_GUILIB_SIXEL_MAX_FPS_RELAX=$(CONFIG_GUILIB_SIXEL_MAX_FPS_RELAX)
CFLAGS += -D CONFIG_GUILIB_SIXEL_RAW_MODE=$(CONFIG_GUILIB_SIXEL_RAW_MODE)
CFLAGS += -D CONFIG_GUILIB_SIXEL_KEYBOARD_ENABLE=$(CONFIG_GUILIB_SIXEL_KEYBOARD_ENABLE)
CFLAGS += -D CONFIG_GUILIB_SIXEL_WARN_UNKNOWN_ESCAPE=$(CONFIG_GUILIB_SIXEL_WARN_UNKNOWN_ESCAPE)

all: py

build:
	mkdir -p build

py-stdlib: build
	$(CC) $(PYFLAGS) -c src/stdguilib.c -o build/stdguilib-py.o
	ar rcs  build/libstdgui-py.a build/stdguilib-py.o
py-lib: build
	$(CC) $(PYFLAGS) -c py/backend.c -o build/backend-py.o
	ar rcs build/libgui-py.a build/backend-py.o
py: py-stdlib py-lib
	$(CC) $(PYFLAGS) -c test.c -o build/test-py.o
	$(LD) -shared build/test-py.o -o executable-py.so -L build -lgui-py -lstdgui-py

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

