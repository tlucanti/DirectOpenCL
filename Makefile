
CFLAGS = -Wall -Wextra -fdiagnostics-color=always -I include
PYFLAGS = $(CFLAGS) -I . -fPIC
SIXFLAGS = $(CFLAGS) -O2 -I ../libsixel/include
STREAMFLAGS = $(CFLAGS) -O0 -g3 -I stream
CC = gcc
LD = gcc

all: stream

py-build:
	$(CC) $(PYFLAGS) -c py/backend.c -o py/backend.o
	$(CC) $(PYFLAGS) -c py/test.c -o py/test.o
	$(LD) -shared py/backend.o py/test.o -o py/libbackend.so

py-tkinter: py-build
	cd py && python3 frontend.py

py-pil: py-build
	cd py && python3 frontend.py

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

