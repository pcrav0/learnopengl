#include "utils.hpp"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

using namespace utils;

int main() {
    {
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
    }
    {
        int *p = alloc<int>();
        *p = 69;

        auto u0 = Unique<int>(p, 1);
        int *u0_ptr = u0.ptr;
        assert(u0.ptr == p);
        auto u1 = std::move(u0);
        assert(u0.ptr == nullptr);
        assert(u1.ptr == u0_ptr);
        u1.destroy();
        assert(u1.ptr == u0.ptr);
        assert(u1.ptr == nullptr);

        int *q = alloc<int>();
        *q = 420;
        auto s0 = Shared<int>(q, 1);
        assert(s0.ptr == q);
        assert(*s0.count == 1);

        auto s1 = s0;
        assert(s0.ptr == s1.ptr);
        assert(s1.ptr != nullptr);

        assert(*s0.count == *s1.count);
        assert(*s1.count == 2);

        s1.destroy();

        assert(s0.ptr == s1.ptr);
        assert(s1.ptr != nullptr);

        assert(*s0.count == *s1.count);
        assert(*s1.count == 1);

        s1.destroy();

        // assert(*s0.count == *s1.count);
        // assert(*s0.count == 0);
    }
    {
        Array<int> a;
        a.push(2);
        a.push(3);
        a.push(69);

        assert(a.length == 3);
        assert(a[0] == 2);
        assert(a[1] == 3);
        assert(a[2] == 69);

        assert(a.shift() == 2);
        assert(a.length == 2);
        assert(a[0] == 3);
        assert(a[1] == 69);

        assert(a.pop() == 69);
        assert(a.length == 1);
        assert(a[0] == 3);
    }
    {
        List<int> l0;
        assert(l0.length() == 0);
    }
    return 0;
}
