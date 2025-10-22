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
make verbose=true config=debug -j$(nproc)
./bin/debug/game
```

## release mode
``` sh
make verbose=true config=release -j$(nproc)
./bin/release/game
```

## tests
``` sh
make verbose=true config=release -j$(nproc)
./bin/test/game
```
