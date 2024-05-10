
CFLAGS = -Wall -Wextra
CFLAGS += -I include
CFLAGS += -O2
CFLAGS += -fdiagnostics-color=always

PYFLAGS = $(CFLAGS) -fPIC
SIXFLAGS = $(CFLAGS) -I thirdparty/sixel
STREAMFLAGS = $(CFLAGS) -I stream
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
CFLAGS += -D CONFIG_GUILIB_SIXEL_MOUSE_ENABLE=$(CONFIG_GUILIB_SIXEL_MOUSE_ENABLE)
CFLAGS += -D CONFIG_GUILIB_SIXEL_WARN_UNKNOWN_ESCAPE=$(CONFIG_GUILIB_SIXEL_WARN_UNKNOWN_ESCAPE)
CFLAGS += -D CONFIG_GUILIB_SIXEL_NO_DRAW=$(CONFIG_GUILIB_SIXEL_NO_DRAW)

BRED     = "\033[01;31m"
BGREEN   = "\033[01;32m"
BYELLOW  = "\033[01;33m"
BBLUE    = "\033[01;34m"
BMAGENTA = "\033[01;35m"
BCYAN    = "\033[01;36m"
BORANGE  = "\033[38;2;255;165;0m"
BRESET   = "\033[0m"

MK_P = printf $(BYELLOW)'MK\t'$(BRESET); echo
CC_P = printf $(BGREEN)'CC\t'$(BRESET); echo
AR_P = printf $(BMAGENTA)'AR\t'$(BRESET); echo
LD_P = printf $(BCYAN)'LD\t'$(BRESET); echo
ELF_P = printf $(BBLUE)'ELF\t'$(BRESET); echo
RM_P = printf $(BRED)'RM\t'$(BRESET); echo
TAR_P = printf $(BORANGE)'TAR\t'$(BRESET); echo

all: thirdparty lib sixel-test stream-test

thirdparty/sixel/libsixel.a:
	@$(MK_P) thirdparty/sixel
	@mkdir -p thirdparty/sixel
	@$(TAR_P) libsixel.tar.gz
	@tar xf thirdparty/libsixel.tar.gz -C thirdparty/sixel

thirdparty: thirdparty/sixel/libsixel.a
.PHONY: thirdparty

thirdparty-clean: thirdparty-sixel-clean
.PHONY: thirdparty-clean

thirdparty-sixel-clean:
	@$(RM_P) thirdparty/sixel
	@rm -rf thirdparty/sixel
.PHONY: thirdparty-sixel-clean

lib: py sixel stream
.PHONY: lib

clean: py-stdgui-clean py-lib-clean py-clean stdgui-clean sixel-clean stream-clean sixel-test-clean stream-test-clean thirdparty-clean
.PHONY: clean

build:
	@$(MK_P) build
	@mkdir -p build

py-stdgui: build
	@$(CC_P) stdguilib-py.o
	@$(CC) $(PYFLAGS) -c src/stdguilib.c -o build/stdguilib-py.o
	@$(AR_P) libstdgui-py.a
	@$(AR)  build/libstdgui-py.a build/stdguilib-py.o
.PHONY: py-stdgui

py-stdgui-clean:
	@$(RM_P) stdguilib-py.o
	@rm -f build/stdguilib-py.o
	@$(RM_P) libstdgui-py.a
	@rm -f build/libstdgui-py.a
.PHONY: py-stdgui-clean

py-lib: build
	@$(CC_P) backend-py.o
	@$(CC) $(PYFLAGS) -c py/backend.c -o build/backend-py.o
	@$(AR_P) libgui-py.a
	@$(AR) build/libgui-py.a build/backend-py.o
.PHONY: py-lib

py-lib-clean:
	@$(RM_P) backend-py.o
	@rm -f build/backend-py.o
	@$(RM_P) libgui-py.a
	@rm -f build/libgui-py.a
.PHONY: py-lib-clean

py: py-stdgui py-lib
	@$(CC_P) test-py.o
	@$(CC) $(PYFLAGS) -c test.c -o build/test-py.o
	@$(LD_P) executable-py.so
	@$(LD) -shared build/test-py.o -o executable-py.so -L build -lgui-py -lstdgui-py
.PHONY: py

py-clean:
	@$(RM_P) test-py.o
	@rm -f build/test-py.o
	@$(RM_P) executable-py.so
	@rm -f executable-py.so
.PHONY: py-clean

stdgui: build
	@$(CC_P) stdguilib.o
	@$(CC) $(CFLAGS) -c src/stdguilib.c -o build/stdguilib.o
	@$(AR_P) stdguilib.o
	@$(AR) libstdgui.a build/stdguilib.o
.PHONY: stdgui

stdgui-clean:
	@$(RM_P) stdguilib.o
	@rm -f build/stdguilib.o
	@$(RM_P) libstdgui.a
	@rm -f libstdgui.a
.PHONY: stdgui-clean

sixel: stdgui
	@$(CC_P) sixel.o
	@$(CC) $(SIXFLAGS) -c sixel/sixel.c -o build/sixel.o
	@$(AR_P) sixel.o
	@$(AR) libgui-sixel.a build/sixel.o thirdparty/sixel/libsixel.a
.PHONY: sixel

sixel-clean:
	@$(RM_P) sixel.o
	@rm -f build/sixel.o
	@$(RM_P) libgui-sixel.a
	@rm -f libgui-sixel.a
.PHONY: sixel-clean

stream: stdgui
	@$(CC_P) encode.o
	@$(CC) $(STREAMFLAGS) -c stream/encode.c -o build/encode.o
	@$(CC_P) netsock.o
	@$(CC) $(STREAMFLAGS) -c stream/netsock.c -o build/netsock.o
	@$(CC_P) stream.o
	@$(CC) $(STREAMFLAGS) -c stream/stream.c -o build/stream.o
	@$(AR_P) libgui-stream.a
	@$(AR) libgui-stream.a build/encode.o build/netsock.o build/stream.o
.PHONY: stream

stream-clean:
	@$(RM_P) encode.o
	@rm -f build/encode.o
	@$(RM_P) netsock.o
	@rm -f build/netsock.o
	@$(RM_P) stream.o
	@rm -f build/stream.o
	@$(RM_P) libgui-stream.a
	@rm -f build/libgui-stream.a
.PHONY: stream-clean

sixel-test: sixel
	@$(ELF_P) guisixel.elf
	@$(CC) $(CFLAGS) -L . \
		test.c \
		-o guisixel.elf \
		-lgui-sixel -lstdgui \
		\
		-L thirdparty/sixel -lsixel
.PHONY: sixel-test

sixel-test-clean:
	@$(RM_P) guisixel.elf
	@rm -f guisixel.elf
.PHONY: sixel-test-clean

stream-test: stream
	@$(ELF_P) guistream.elf
	@$(CC) $(CFLAGS) -L . \
		test.c \
		-o guistream.elf \
		-lgui-stream -lstdgui \
		\
		-ljpeg
.PHONY: stream-test

stream-test-clean:
	@$(RM_P) guistream.elf
	@rm -f guistream.elf
.PHONY: stream-test-clean

