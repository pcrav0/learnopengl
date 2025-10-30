#define UTILS_MACRO_RENAME
#define UTILS_IMPLEMENT
#include "utils.hpp"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

using namespace utils;

int main() {
    USE[] {
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
    };
    USE[] {
        int *p = alloc<int>();
        *p = 69;

        auto u0 = Unique<int>(p, 1);
        int *u0_ptr = u0.addr();
        assert(u0.addr() == p);
        auto u1 = std::move(u0);
        assert(u0.addr() == nullptr);
        assert(u1.addr() == u0_ptr);
        u1.destroy();
        assert(u1.addr() == u0.addr());
        assert(u1.addr() == nullptr);

        int *q = alloc<int>();
        *q = 420;
        auto s0 = Shared<int>(q, 1);
        assert(s0.addr() == q);
        assert(s0.counter() == 1);

        auto s1 = s0;
        assert(s0.addr() == s1.addr());
        assert(s1.addr() != nullptr);

        assert(s0.counter() == s1.counter());
        assert(s1.counter() == 2);

        // s1.destroy();

        // assert(s0.addr() == s1.addr());
        // assert(s1.addr() != nullptr);

        // assert(*s0.counter() == *s1.counter());
        // assert(*s1.counter() == 1);

        // s1.destroy();

        // assert(*s0.counter() == *s1.counter());
        // assert(*s0.counter() == 0);
    };
    USE[] {
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
    };
    USE[] {
        List<int> l0;
        assert(l0.length() == 0);

        l0.prepend(3);
        assert(l0.length() == 1);
        assert(l0.at(0).head == 3);

        l0.prepend(69);
        assert(l0.length() == 2);
        assert(l0.at(0).head == 69);
        assert(l0.at(1).head == 3);

        l0.append(420);
        assert(l0.length() == 3);
        assert(l0.at(0).head == 69);
        assert(l0.at(1).head == 3);
        assert(l0.at(2).head == 420);

        l0.append(42);
        assert(l0.length() == 4);
        assert(l0.at(0).head == 69);
        assert(l0.at(1).head == 3);
        assert(l0.at(2).head == 420);
        assert(l0.at(3).head == 42);

        l0.prepend(-1);
        l0.forward();
        assert(l0.length() == 4);
        assert(l0.at(0).head == 69);
        assert(l0.at(1).head == 3);
        assert(l0.at(2).head == 420);
        assert(l0.at(3).head == 42);

        l0.remove(1);
        assert(l0.length() == 3);
        assert(l0.at(0).head == 69);
        assert(l0.at(1).head == 420);
        assert(l0.at(2).head == 42);

        l0.remove(2);
        assert(l0.length() == 2);
        assert(l0.at(0).head == 69);
        assert(l0.at(1).head == 420);

        l0.forward();
        assert(l0.length() == 1);
        assert(l0.at(0).head == 420);

        l0.forward();
        assert(l0.length() == 0);
    };
    USE[] {
        auto l0 = Tree<int>();
        assert(l0.length_next() == 0);
        assert(l0.length_prev() == 0);
        assert(l0.length() == 0);

        l0.prepend_next(420);
        assert(l0.length_next() == 0);
        assert(l0.length_prev() == 0);
        assert(l0.length() == 1);

        l0.prepend_next(69);
        assert(l0.length_next() == 1);
        assert(l0.length_prev() == 0);
        assert(l0.length() == 2);

        l0.prepend_next(-420);
        assert(l0.length_next() == 2);
        assert(l0.length_prev() == 0);
        assert(l0.length() == 3);

        l0.prepend_prev(-69);
        assert(l0.length_next() == 2);
        assert(l0.length_prev() == 1);
        assert(l0.length() == 4);

        l0.prepend_prev(2);
        assert(l0.length_next() == 2);
        assert(l0.length_prev() == 2);
        assert(l0.length() == 5);

        assert(l0.at(-2).head == -420);
        assert(l0.at(-1).head == -69);
        assert(l0.at(0).head == 2);
        assert(l0.at(1).head == 69);
        assert(l0.at(2).head == 420);

        l0.append_next(911);
        l0.append_prev(-911);
        assert(l0.remove(3).head == 911);
        assert(l0.remove(-3).head == -911);

        assert(l0.forward().head == 2);
        assert(l0.remove(1).head == 420);
        assert(l0.backward().head == 69);
        assert(l0.remove(-1).head == -420);
        assert(l0.remove().head == -69);
    };
    USE[] {
        auto t0 = Table<String, int>();

        // t0["hi"_s] = 5;
        t0.put({"hi"_s, 5});
        assert(t0.get("hi"_s));
        assert(*t0.get("hi"_s) == 5);

        t0["other"_s] = 3;
        assert(t0["hi"_s] == 5);
        assert(t0["other"_s] == 3);

        t0["hi"_s] = 69;
        assert(t0["hi"_s] == 69);
        assert(t0["other"_s] == 3);
    };
    USE[] {
        int i = 0;
        const int end = 5;
        auto l0 = Array<int>();
        l0.reserve(abs(end - i));
        for (int it = i; it < end; it++)
            l0.push(it);

        for (auto &it : l0) {
            assert(it == i++);
        }

        assert(i == end);
        for (auto &it : Reverse(l0)) {
            assert(it == --i);
        }
    };
    USE[] {
        int i = 0;
        const int end = 5;
        auto l0 = List<int>();
        for (int it = i; it < end; it++)
            l0.append(it);

        for (auto &it : l0) {
            assert(it.head == i++);
        }
    };
    USE[] {
        int i = 1;
        const int end = 5;
        auto l0 = Tree<int>();
        l0.prepend_next(0);

        for (int it = i; it < end / 2 + 1; it++)
            l0.append_next(it);

        for (int it = i; it < end / 2 + 1; it++)
            l0.append_prev(-it);

        i = -(end / 2);
        for (auto &it : l0) {
            assert(it.head == i++);
        }

        assert(i == end / 2);
        for (auto &it : Reverse(l0)) {
            assert(it.head == --i);
        }
    };
    USE[] {
        int i = 0;
        const int end = 5;

        auto l0 = Table<String, int>();
        String keys[end] = {"foo"_s, "bar"_s, "buzz"_s, "quux"_s, "xyzzy"_s};

        l0.reserve(abs(end - i));
        for (int it = i; it < end; it++) {
            l0[keys[it]] = it;
        }

        for (auto &it : l0) {
            assert(l0[it.key] == it.value);
        }

        for (auto &it : Reverse(l0)) {
            assert(l0[it.key] == it.value);
        }

        // assert(l0.is_sorted());
    };
    USE[] {
        auto l0 = Table<String, int>();
        l0.put({{"nice"_s, 69}, {"UwU"_s, 42}, {"poggers"_s, 1337}});
        // assert(l0.is_sorted());
        assert(l0["nice"_s] == 69);
        assert(l0["UwU"_s] == 42);
        assert(l0["poggers"_s] == 1337);
    };

    puts("******");
    printf("nice\n");
    return 0;
}
