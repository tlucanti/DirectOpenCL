#!/bin/bash

if [[ $GUILIB == '' ]]; then
        echo '============================== [GUILIB ERROR] ==============================' >&2
        echo 'env $GUILIB should be set to "sixel" or "stream"' >&2
        echo '============================================================================' >&2
        exit 1
elif [[ $GUILIB == 'sixel' ]]; then
        echo -n ' -lgui-sixel -lsixel '
elif [[ $GUILIB == 'stream' ]]; then
        echo -n ' -lgui-stream -ljpeg '
else
        echo '============================== [GUILIB ERROR] ==============================' >&2
        echo 'invalid GUILIB type' >&2
        echo '============================================================================' >&2
        exit 1
fi

