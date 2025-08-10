#!/bin/bash

for arg in "$@"; do declare $arg='1'; done

if [ ! -v release ]; then debug=1; fi
if [ ! -v snake ] && [ ! -v poe ] && [ ! -v sam ]; then all=1; fi

if [ -v debug ]; then echo "[debug mode]"; fi
if [ -v release ]; then echo "[release mode]"; fi

gcc_common="-Wextra -Wall -DOS_LINUX -DWM_X11 -DGPU_OPENGL"
gcc_link="-lX11 -lGL -lEGL"
gcc_out="-o"
gcc_debug="gcc -g -Og $gcc_common"
gcc_release="gcc -Ofast $gcc_common"

if [ -v debug ]; then compile=$gcc_debug; fi
if [ -v release ]; then compile=$gcc_release; fi
link=$gcc_link
out=$gcc_out

mkdir -p build
if [ -v all ]; then snake=1 poe=1 sam=1; fi
if [ -v snake ]; then echo "compiling snake"; $compile main_snake.c $link $out build/snake; fi
if [ -v poe ]; then echo "compiling poe scratch"; $compile main_poe_scratch.c $link $out build/poe_scratch; fi
if [ -v sam ]; then echo "compiling sam scratch"; $compile main_sam_scratch.c $link $out build/sam_scratch; fi
