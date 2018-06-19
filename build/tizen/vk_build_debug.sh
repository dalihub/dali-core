#!/bin/sh

autoreconf --install
./configure --prefix=$DESKTOP_PREFIX --enable-vulkan=1.0 --enable-debug
make install -j8
