#include <assert.h>
#include <stdio.h>

#include "utils.hpp"

using namespace utils;

int texture_main();
int transformation_main();
int cord_main();

Table<String, int (*)(void)> mains;

void help() {
    printf("valid main names:\n");

    for (const auto &[key, value] : mains) {
        printf("- %.*s\n", (int)key.length, key.items);
    }

    exit(-1);
}

int main(int argc, char **argv) {
    mains["texture"_s] = texture_main;
    mains["transformation"_s] = transformation_main;
    mains["cord"_s] = cord_main;
    mains["tr"_s] = transformation_main;
    mains["te"_s] = texture_main;

    if (argc < 2) {
        help();
    }

    String arg = string_from_zstr(argv[1]);
    if (auto f = mains.get(arg); f) {
        return (*f)();
    } else {
        help();
    }
}
