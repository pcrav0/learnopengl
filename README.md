# libs
- luajit 5.2
- box2d.org
- imgui

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
make -f game.make verbose=true config=debug -j$(nproc)
./bin/debug/main texture
```

## release mode
``` sh
make -f game.make verbose=true config=release -j$(nproc)
./bin/release/main texture
```

## tests
``` sh
make -f test.make verbose=true config=test -j$(nproc)
./bin/test/test
```

# credits
- https://gist.github.com/MangaD/fde1290be3e8b362ed524117382e9a47
- https://github.com/Javier-varez/Ditto
