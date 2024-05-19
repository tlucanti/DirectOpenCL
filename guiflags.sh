#!/bin/bash

# give path to the root of GUIlib directory to $1 argument, otherwise it will
# default root directory to "../GUIlib" relative path

if [[ $1 == '' ]]; then
	export GUIPATH="../GUIlib"
else
	export GUIPATH=$1
fi

if [[ $GUILIB == '' ]]; then
        echo '============================== [GUILIB ERROR] ==============================' >&2
        echo 'env $GUILIB should be set to "opengl", "sixel" or "stream"' >&2
        echo '============================================================================' >&2
	echo 'or set variable value manually:' >&2
	read -p ">>> " GUILIB
fi

if [[ $GUILIB == 'sixel' ]]; then
        echo -n " -L $GUIPATH -L $GUIPATH/thirdparty/sixel -lgui-sixel -lsixel "
elif [[ $GUILIB == 'stream' ]]; then
        echo -n " -L $GUIPATH -lgui-stream -ljpeg "
elif [[ $GUILIB == 'opengl' ]]; then
	echo -n " -L $GUIPATH -lgui-opengl -lOpenGL -lglfw "
else
        echo '============================== [GUILIB ERROR] ==============================' >&2
        echo 'invalid GUILIB type: ' $GUILIB ', expected "opengl", "sixel" or "stream" '>&2
        echo '============================================================================' >&2
        exit 1
fi

