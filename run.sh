#!/bin/sh

if [ -z "$1" ]; then
    echo '??? no'
    exit 1
fi

prefix=''
type nixGL && prefix='nixGL'

set -xe

premake5 gmake
time make CXX=clang++ -f game.make -j$(nproc) verbose=true config=debug "$1"
$prefix ./bin/debug/"$1"
