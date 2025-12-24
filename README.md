# libs
- luajit 5.2
- box2d.org
- imgui
- https://github.com/orangeduck/tgc

# tools
## 2d
- aseprite
- krita
- tiled
- trench broom

## 3d
- trenchbroom
- blender

# TODO
- [ ] https://en.wikipedia.org/wiki/Block_sort
- [ ] lazy lits

# test for memory leaks and use after free
```
valgrind --tool=memcheck --leak-check=yes ./bin/debug/game
```


# run
## generate the make files
``` sh
premake5 gmake 
```

## debug mode
``` sh
./run.sh texture
```

## release mode
``` sh
make -f game.make verbose=true config=release -j$(nproc)
./bin/release/main texture
```

## tests
``` sh
make -f test.make verbose=true config=test -j$(nproc) && ./bin/test/test
```

# credits
- https://learnopengl.com/
- https://gist.github.com/MangaD/fde1290be3e8b362ed524117382e9a47
- https://github.com/Javier-varez/Ditto
- https://gist.github.com/liam-middlebrook/c52b069e4be2d87a6d2f
