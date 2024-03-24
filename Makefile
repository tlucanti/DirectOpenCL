
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

sixlib:
	gcc \
		-Wall -Wextra \
		-O2 -fdiagnostics-color=always \
		-I ../libsixel/include \
		-I include \
		sixel/sixel.c \
		-c -o sixel/sixel.o
	ar rcs libgui.a sixel/sixel.o

stdsix:
	gcc \
		-Wall -Wextra \
		-O2 -fdiagnostics-color=always \
		-I include \
		src/stdguilib.c \
		-c -o src/stdguilib.o
	ar rcs libstdgui.a src/stdguilib.o

six: sixlib stdsix
	gcc \
		-Wall -Wextra \
		-O2 -fdiagnostics-color=always \
		-I include \
		-L . \
		-L ../libsixel/src/.libs \
		test.c \
		-lgui -lstdgui -lsixel \
		-o guisixel

