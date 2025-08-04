#!/bin/bash

# NOTE(poe): Unpack CLI args (ensures that the -v "if" checks work)
for arg in "$@"; do declare $arg='1'; done

# NOTE(poe): Set defaults for unspecified settings
if [ ! -v release ]; then debug=1; fi
if [ ! -v snake ] && [ ! -v poe ] && [ ! -v sam ]; then all=1; fi

if [ -v debug ]; then echo "[debug mode]"; fi
if [ -v release ]; then echo "[release mode]"; fi

gcc_common="-Wextra -Wall -lX11"
gcc_debug="gcc -g -Og $gcc_common"
gcc_release="gcc -Ofast $gcc_common"

if [ -v debug ]; then compile=$gcc_debug; fi
if [ -v release ]; then compile=$gcc_release; fi

mkdir -p build
cd build
if [ -v all ]; then snake=1 poe=1 sam=1; fi
if [ -v snake ]; then echo "compiling snake"; $compile ../main_snake.c -o snake; fi
if [ -v poe ]; then echo "compiling poe scratch"; $compile ../main_poe_scratch.c -o poe_scratch; fi
if [ -v sam ]; then echo "compiling sam scratch"; $compile ../main_sam_scratch.c -o sam_scratch; fi
cd ..
