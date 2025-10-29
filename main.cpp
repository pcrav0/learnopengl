int texture_main();

#include <assert.h>
#include <stdio.h>

#include "utils.hpp"

using namespace utils;

int main(int argc, char **argv) {
    assert(argc >= 2);

    String arg = string_from_zstr(argv[1]);

    if (arg == "texture"_s) {
        return texture_main();
    } else {
        printf("%s dont exists\n", arg.zstr());
        return -1;
    }
}
