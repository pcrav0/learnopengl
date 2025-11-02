if [ -z "$1" ]; then
    echo '??? no'
    exit 1
fi

set -xe

premake5 gmake
time make CXX=clang++ -f game.make -j$(nproc) verbose=true config=debug "$1"
./bin/debug/"$1"
