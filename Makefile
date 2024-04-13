
CFLAGS = -Wall -Wextra -fdiagnostics-color=always -I include
PYFLAGS = $(CFLAGS) -fPIC
SIXFLAGS = $(CFLAGS) -O2 -I ../libsixel/include
STREAMFLAGS = $(CFLAGS) -O0 -g3 -I stream
CC = gcc
LD = gcc
AR = ar rcs

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

all: lib sixel-test stream-test

lib: py sixel stream

clean:
	rm -rf build
	rm -f executable-py.so
	rm -f guisixel
	rm -f guistream
	rm -f libgui-sixel.a
	rm -f libgui-stream.a
	rm -f libstdgui.a

build:
	mkdir -p build

py-stdgui: build
	$(CC) $(PYFLAGS) -c src/stdguilib.c -o build/stdguilib-py.o
	$(AR)  build/libstdgui-py.a build/stdguilib-py.o

py-lib: build
	$(CC) $(PYFLAGS) -c py/backend.c -o build/backend-py.o
	$(AR) build/libgui-py.a build/backend-py.o

py: py-stdgui py-lib
	$(CC) $(PYFLAGS) -c test.c -o build/test-py.o
	$(LD) -shared build/test-py.o -o executable-py.so -L build -lgui-py -lstdgui-py

stdgui: build
	$(CC) $(CFLAGS) -c src/stdguilib.c -o build/stdguilib.o
	$(AR) libstdgui.a build/stdguilib.o

sixel: stdgui
	$(CC) $(SIXFLAGS) -c sixel/sixel.c -o build/sixel.o
	$(AR) libgui-sixel.a build/sixel.o
.PHONY: sixel

stream: stdgui
	$(CC) $(STREAMFLAGS) -c stream/encode.c -o build/encode.o
	$(CC) $(STREAMFLAGS) -c stream/netsock.c -o build/netsock.o
	$(CC) $(STREAMFLAGS) -c stream/stream.c -o build/stream.o
	$(AR) libgui-stream.a build/encode.o build/netsock.o build/stream.o
.PHONY: stream

sixel-test: sixel
	$(CC) $(CFLAGS) -L . \
		test.c \
		-o guisixel \
		-lgui-sixel -lstdgui \
		\
		-L thirdparty -lsixel

stream-test: stream
	$(CC) $(CFLAGS) -L . \
		test.c \
		-o guistream \
		-lgui-stream -lstdgui \
		\
		-ljpeg

