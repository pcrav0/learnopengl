#include "utils.hpp"

#include <assert.h>
#include <stdlib.h>

using namespace utils;

int main() {
    int *p = alloc<int>();
    assert(p != NULL);
    *p = 5;
    assert(*p == 5);

    int *p0 = alloc<int>(100);
    assert(p0 != nullptr);

    int *q = realloc(p, 1, 10);
    assert(p != q);
    q[1] = 8;
    assert(q[0] == 5);
    assert(q[1] == 8);
    assert(p[0] == 5);

    // auto zero = Slice<int>();
    // assert(zero.items == nullptr);
    // assert(zero.length == 0);

    // auto s1 = Slice<int>((int *)malloc(sizeof(int) * 3), 3);
    // assert(s1.items != nullptr);
    // assert(s1.length == 3);
    // s1[0] = 69;
    // s1[1] = 420;
    // s1[2] = 1337;

    // assert(s1.items[0] == 69);
    // assert(s1[0] == 69);

    // s1.swap(0, 2);

    // assert(s1[0] == 1337);
    // s1.swap(0, -1);

    // assert(s1[0] == 69);

    // Slice<int> s = Slice<int>({1, 2, 3, 4});
    // s.at(0) = 3;

    // int a = 3;
    // int b = 5;
    // swap(a, b);
    // assert(a == 5);
    // assert(b == 3);

    return 0;
}
