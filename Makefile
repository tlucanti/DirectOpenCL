
CFLAGS = -Wall -Wextra -I . -fPIC
CC = gcc
LD = gcc

all: six

py-build:
	$(CC) $(CFLAGS) -c py/backend.c -o py/backend.o
	$(CC) $(CFLAGS) -c py/test.c -o py/test.o
	$(LD) -shared py/backend.o py/test.o -o py/libbackend.so

py-tkinter: py-build
	cd py && python3 frontend.py

py-pil: py-build
	cd py && python3 frontend.py

six:
	gcc \
		-Wall -Wextra \
		-O0 -g3 -fdiagnostics-color=always \
		-I ../libsixel/include \
		-I include \
		-L ../libsixel/src/.libs/ \
		sixel/sixel.c \
		src/stdguilib.c \
		-lsixel \
		-o guisixel

