#!/bin/bash

if [[ $GUILIB == '' ]]; then
        echo '============================== [GUILIB ERROR] ==============================' >&2
        echo 'env $GUILIB should be set to "opengl", "sixel" or "stream"' >&2
        echo '============================================================================' >&2
	echo 'or set variable value manually:' >&2
	read -p ">>> " GUILIB
fi

if [[ $GUILIB == 'sixel' ]]; then
        echo -n ' -L ../GUIlib -L ../GUIlib/thirdparty/sixel -lgui-sixel -lsixel '
elif [[ $GUILIB == 'stream' ]]; then
        echo -n ' -L ../GUIlib -lgui-stream -ljpeg '
elif [[ $GUILIB == 'opengl' ]]; then
	echo -n ' -L ../GUIlib -lgui-opengl -lOpenGL -lglfw '
else
        echo '============================== [GUILIB ERROR] ==============================' >&2
        echo 'invalid GUILIB type: ' $GUILIB ', expected "sixel" or "stream" '>&2
        echo '============================================================================' >&2
        exit 1
fi

