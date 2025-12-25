#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <stdio.h>

#include <utility>

#include <linux/limits.h> // MAX_PATH

#define UTILS_IGNORE(...)

#ifndef UTILS_PROC
#define UTILS_PROC
#endif

#ifndef UTILS_PROC_INIT
#define UTILS_PROC_INIT constexpr __attribute__((constructor))
#endif

#ifndef UTILS_PROC_FINI
#define UTILS_PROC_FINI constexpr __attribute__((destructor))
#endif

#ifndef UTILS_PROC_CONSTRUCT
#define UTILS_PROC_CONSTRUCT UTILS_PROC explicit
#endif

#ifndef UTILS_PROC_CONSTRUCT_IMPLICITY
#define UTILS_PROC_CONSTRUCT_IMPLICITY UTILS_PROC
#endif

#ifndef UTILS_PROC_CONSTRUCT_COPY
#define UTILS_PROC_CONSTRUCT_COPY UTILS_PROC
#endif

#ifndef UTILS_PROC_CONSTRUCT_MOVE
#define UTILS_PROC_CONSTRUCT_MOVE UTILS_PROC
#endif

#ifndef UTILS_PROC_DESTRUCT
#define UTILS_PROC_DESTRUCT UTILS_PROC virtual
#endif

#ifndef UTILS_MAX_CONTEXT_ALLOCATORS
#define UTILS_MAX_CONTEXT_ALLOCATORS 8
#endif

#ifndef UTILS_ASSER
#include <assert.h>
#define UTILS_ASSERT(X) assert((X))
#endif

#ifndef UTILS_BREAKPOINT
#define UTILS_BREAKPOINT() __asm__("int3")
#endif

#ifndef UTILS_UNREACHEBLE
#define UTILS_UNREACHEBLE() assert(false)
#endif

#define UTILS_VA_ARGS_HEAD(HEAD, ...) HEAD
#define UTILS_VA_ARGS_TAIL(_HEAD, ...) __VA_ARGS__

#define UTILS__CONCAT(X, Y) X##Y
#define UTILS_CONCAT(X, Y) UTILS__CONCAT(X, Y)

#define UTILS__STRINGFY(X) #X
#define UTILS_STRINGFY(X) UTILS__STRINGFY(X)

#define UTILS_ARRAY_LENGTH(X) (sizeof((X)) / sizeof((X)[0]))

#define UTILS_DEFER                                                            \
    utils::_Defer UTILS_CONCAT(UTILS_CONCAT(__defer__, __COUNTER__), __LINE__) =

#define UTILS_USE                                                              \
    utils::_Use UTILS_CONCAT(UTILS_CONCAT(__use__, __COUNTER__), __LINE__) =

#ifdef UTILS_MACRO_RENAME
#define VA_ARGS_HEAD UTILS_VA_ARGS_HEAD
#define VA_ARGS_TAIL UTILS_VA_ARGS_TAIL
#define CONCAT UTILS_CONCAT
#define STRINGFY UTILS_STRINGFY
#define ARRAY_LENGTH UTILS_ARRAY_LENGTH
#define DEFER UTILS_DEFER
#define USE UTILS_USE
#endif

namespace utils {

typedef long long longlong;

typedef ptrdiff_t ssize;
typedef size_t usize;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

struct Allocator;

extern "C" {
extern u64 random_state;

UTILS_PROC f32 random_norm();
UTILS_PROC ssize ptr_diff(void *a, void *b);

UTILS_PROC bool bitf_get(const u8 *n, u8 index);
UTILS_PROC void bitf_set(u8 *n, u8 index, bool bit);
UTILS_PROC void bitf_on(u8 *n, u8 index);
UTILS_PROC void bitf_off(u8 *n, u8 index);
UTILS_PROC void bitf_toggle(u8 *n, u8 index);
};

UTILS_PROC void context_allocator_push(Allocator *allocator);
UTILS_PROC Allocator *context_allocator_replace(Allocator *allocator);
UTILS_PROC Allocator *context_allocator_pop();
UTILS_PROC Allocator *context_allocator_peek();

UTILS_PROC void context_allocator_create(Allocator &&allocator);
UTILS_PROC void context_allocator_destroy();

UTILS_PROC void context_allocator_reset();

template <typename T> UTILS_PROC T min(T x) { return x; }

template <typename T, typename... Rest> UTILS_PROC T min(T x, Rest... rest) {
    auto y = min(rest...);
    return x < y ? x : y;
}

template <typename T> UTILS_PROC T max(T x) { return x; }

template <typename T, typename... Rest> UTILS_PROC T max(T x, Rest... rest) {
    auto y = max(rest...);
    return x < y ? y : x;
}

template <typename T> T *non_nil(T *ptr) {
    static T fallback = {};
    memset(&fallback, 0, sizeof(T));
    return ptr == nullptr ? &fallback : ptr;
}

template <typename T> UTILS_PROC T clamp(T x, T low, T high) {
    return min(max(low, x), high);
}

template <typename T> UTILS_PROC T mod(T a, T b) { return (a % b + b) % b; }

template <typename T> UTILS_PROC void swap(T &a, T &b) {
    T tmp = a;
    a = b;
    b = tmp;
}

// TODO: check if is needed to use memmove or something like that
template <typename T> UTILS_PROC T *copy(T *src, T *dest, usize length = 1) {
    if (src == dest)
        return src;

    return static_cast<T *>(memcpy(static_cast<T *>(dest),
                                   static_cast<T *>(src), length * sizeof(T)));
}

template <typename F> struct _Defer {
    F f;
    _Defer(F f) : f(f) {}
    ~_Defer() { f(); }
};

template <typename F> struct _Use {
    _Use(F f) { f(); }
};

template <typename T> struct Reverse {
    struct iterator {
        T::iterator_type it;

        UTILS_PROC_CONSTRUCT iterator(T::iterator_type it) : it(it) {}

        UTILS_PROC iterator &operator++() {
            --it;
            return *this;
        }

        UTILS_PROC iterator operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        UTILS_PROC iterator &operator--() {
            ++it;
            return *this;
        }

        UTILS_PROC iterator operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        UTILS_PROC bool operator==(const iterator &other) const {
            return it == other.it;
        }

        UTILS_PROC T::iterator_it_type &operator*() { return *it; }
        UTILS_PROC T::iterator_it_type *operator->() { return it; }
    };

    T container;
    UTILS_PROC iterator begin() { return iterator(--container.end()); }
    UTILS_PROC iterator end() { return iterator(--container.begin()); }
};

template <typename T, typename U> struct Pair {
    T key;
    U value;

    UTILS_PROC_CONSTRUCT_IMPLICITY Pair(T key, U value)
        : key(key), value(value) {}

    s64 operator<=>(const Pair &other) const { return key <=> other.key; }
};

template <typename T> struct Maybe {
    T value;
    bool has;

    UTILS_PROC T operator*() {
        UTILS_ASSERT(has);
        return value;
    }

    UTILS_PROC operator bool() { return has; }
};

template <typename T> Maybe<T> UTILS_PROC some(T value) {
    return Maybe<T>{
        .value = value,
        .has = true,
    };
}

template <typename T> Maybe<T> UTILS_PROC none() {
    return Maybe<T>{
        .value = {},
        .has = false,
    };
}

template <typename Value, typename Error> struct Result {
    Value value;
    Error error;
    bool has_value;
};

template <typename Value, typename Error>
UTILS_PROC Result<Value, Error> result_value(Value value);

template <typename Value, typename Error>
UTILS_PROC Result<Value, Error> result_error(Error value);

struct Allocator {
    UTILS_PROC virtual void *run(void *ptr, usize old_size, usize new_size) = 0;
    UTILS_PROC virtual void destroy() = 0;
    UTILS_PROC virtual void reset() = 0;

    UTILS_PROC_DESTRUCT ~Allocator() {}
};

template <typename T>
UTILS_PROC T *alloc(uint count = 1,
                    Allocator *allocator = context_allocator_peek()) {
    UTILS_ASSERT(allocator != nullptr);
    return static_cast<T *>(allocator->run(nullptr, 0, sizeof(T) * count));
}

template <typename T>
UTILS_PROC void dealloc(T *ptr, uint count,
                        Allocator *allocator = context_allocator_peek()) {
    UTILS_ASSERT(allocator != nullptr);
    if (ptr == nullptr || count == 0)
        return;
    allocator->run(ptr, sizeof(T) * count, 0);
}

template <typename T>
UTILS_PROC T *realloc(T *ptr, uint old_count, uint new_count,
                      Allocator *allocator = context_allocator_peek()) {
    UTILS_ASSERT(allocator != nullptr);
    return static_cast<T *>(
        allocator->run(ptr, sizeof(T) * old_count, sizeof(T) * new_count));
}

template <typename T> struct Slice {
    struct iterator {
        T *current;

        UTILS_PROC_CONSTRUCT iterator(T *ptr) : current(ptr) {}

        UTILS_PROC bool operator==(const iterator &other) const {
            return current == other.current || current == nullptr ||
                   other.current == nullptr;
        }

        UTILS_PROC bool operator!=(const iterator &other) const {
            return !(*this == other);
        }

        UTILS_PROC iterator &operator++() {
            ++current;
            return *this;
        }

        // UTILS_PROC iterator &operator++(T) {
        //     iterator result = *this;
        //     ++(*this);
        //     return result;
        // }

        UTILS_PROC iterator &operator--() {
            --current;
            return *this;
        }

        // UTILS_PROC iterator &operator--(T) {
        //     iterator result = *this;
        //     --(*this);
        //     return result;
        // }

        UTILS_PROC T &operator*() { return *current; }
        UTILS_PROC T *operator->() { return current; }
    };

    using iterator_it_type = T;
    using iterator_type = iterator;

    T *items;
    usize length;

    UTILS_PROC_CONSTRUCT Slice() : items(nullptr), length(0) {}

    UTILS_PROC_CONSTRUCT Slice(T *items, usize length)
        : items(items), length(length) {}

    UTILS_PROC_CONSTRUCT Slice(std::initializer_list<T> values)
        : length(values.size()) {
        items = (T *)values.begin();
    }

    UTILS_PROC T &at_raw(ssize index) const { return items[index]; }

    UTILS_PROC T &at(ssize index) const {
        if (index < 0)
            index += length;

        return at_raw(index);
    }

    UTILS_PROC T &at_ring(ssize index) const {
        return at(mod<ssize>(index, length));
    }

    UTILS_PROC T &operator[](usize index) const { return at(index); }

    UTILS_PROC iterator begin() const { return iterator(&at(0)); }
    UTILS_PROC iterator end() const { return iterator(&at_raw(length)); }

    UTILS_PROC int hash() const {
        int result = 6432;

        for (const auto &it : *this) {
            result += it << 5;
        }

        return result;
    }

    // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
    UTILS_PROC int operator<=>(const Slice &other) const {
        return hash() - other.hash();
    }

    UTILS_PROC bool operator==(const Slice &other) const {
        if (other.length != length)
            return false;

        for (usize i = 0; i < length; i++) {
            if (at(i) != other.at(i))
                return false;
        }

        return true;
    }

    UTILS_PROC bool operator!=(const Slice &other) const {
        return !(*this == other);
    }

    UTILS_PROC Slice splice(ssize start, ssize end) const {
        if (start < 0)
            start += length + 1;

        if (end < 0)
            end += length + 1;

        // UTILS_ASSERT(mod<ssize>(end, (length + 1)) >
        //              mod<ssize>(start, (length + 1)));
        // UTILS_ASSERT(start <= length);
        // UTILS_ASSERT(end <= length);

        return Slice(&at(start), end - start);
    }

    UTILS_PROC Slice swap(ssize a, ssize b) {
        if (a == b)
            return *this;

        utils::swap(at(a), at(b));
        return *this;
    }

    UTILS_PROC Slice reverse() {
        for (uint i = 0; i < length / 2; i++) {
            swap(i, length - 1 - i);
        }

        return *this;
    }

    UTILS_PROC Slice rotate(ssize n) {
        reverse();
        splice(0, n).reverse();
        splice(n, -1).reverse();

        return *this;
    }

    UTILS_PROC Slice sort(int (*cmp)(const void *a, const void *b) =
                              [](const void *a, const void *b) -> int {
        auto cmp = static_cast<const T *>(a) <=> static_cast<const T *>(b);
        return *reinterpret_cast<int *>(&cmp);
    }) {
        std::qsort(items, length, sizeof(T), cmp);

        return *this;
    }

    UTILS_PROC bool is_sorted(bool left = true) const {
        if (length <= 1)
            return true;

        for (uint i = 0; i < length - 1; i++) {
            auto cmp = at(i) <=> at(i + 1);
            if ((left && cmp > 0) || (!left && cmp < 0))
                return false;
        }

        return true;
    }

    UTILS_PROC usize sorted_index(const T &value) const {
        UTILS_ASSERT(is_sorted(true) || is_sorted(false));

        usize low = 0, high = length;

        while (low < high) {
            usize mid = (low + high) / 2;

            if (at(mid) < value)
                low = mid + 1;
            else
                high = mid;
        }

        return low;
    }

    UTILS_PROC Maybe<usize> binary_search(const T &value) const {
        UTILS_ASSERT(is_sorted(true) || is_sorted(false));

        usize low = 0, high = length;

        while (low < high) {
            usize mid = (low + high) / 2;

            if (at(mid) == value)
                return some(mid);

            if (at(mid) < value)
                low = mid + 1;
            else
                high = mid;
        }

        return none<usize>();
    }

    UTILS_PROC Slice push_ring(const T &value) {
        rotate(-2);
        at(-1) = value;
        return *this;
    }

    UTILS_PROC Slice unshift_ring(const T &value) {
        rotate(1);
        at(0) = value;
        return *this;
    }
};

template <typename T> struct Array : public Slice<T> {
    usize capacity;

    using Base = Slice<T>;

    static constexpr usize init_capacity = (256 / sizeof(T)) > 0
                                               ? (256 / sizeof(T))
                                               : 1;

    UTILS_PROC_CONSTRUCT Array() : Slice<T>(), capacity{0} {}

    UTILS_PROC_CONSTRUCT Array(T *items, usize length)
        : Slice<T>(items, length), capacity{length} {}

    // UTILS_PROC Array(std::initializer_list<T> values)
    //     : items{const_cast<T *>(values.begin())}, length{values.size()} {}

    UTILS_PROC Slice<T> slice() const {
        return Slice<T>(Base::items, Base::length);
    }

    UTILS_PROC Array reserve(usize increment,
                             Allocator *allocator = context_allocator_peek()) {
        if (Base::length + increment <= capacity)
            return *this;

        usize old_size = capacity;
        if (capacity == 0)
            capacity = init_capacity;
        while (Base::length + increment > capacity)
            capacity *= 2;

        Base::items = realloc<T>(Base::items, old_size, capacity, allocator);
        return *this;
    }

    UTILS_PROC T &
    insert_reserve(usize index,
                   Allocator *allocator = context_allocator_peek()) {
        UTILS_ASSERT(index <= Base::length);
        reserve(index + 1, allocator);

        Base::length++;
        memset(&Base::at(-1), 0, sizeof(T));

        if ((Base::length - 1) != index)
            Base::splice(index, Base::length).rotate(1);

        return Base::at_raw(index);
    }

    UTILS_PROC Array insert(const T &item, usize index,
                            Allocator *allocator = context_allocator_peek()) {
        insert_reserve(index, allocator) = item;
        return *this;
    }

    UTILS_PROC T remove(ssize index,
                        Allocator *allocator = context_allocator_peek()) {
        T result = Base::at(index);

        if (index < 0)
            index += Base::length + 1;

        if (index != Base::length)
            Base::splice(index, Base::length).rotate(-2);
        Base::length--;
        return result;
    }

    UTILS_PROC T &
    sorted_reserve(const T &item, Allocator *allocator = context_allocator_peek()) {
        return insert_reserve(Base::sorted_index(item), allocator);
    }

    UTILS_PROC Array
    sorted_insert(const T &item, Allocator *allocator = context_allocator_peek()) {
        sorted_reserve(item, allocator) = item;
        return *this;
    }

    UTILS_PROC Array sorted_remove(const T &item, Allocator *allocator = context_allocator_peek()) {
        if (!binary_search(item))
            return *this;

        remove(sorted_index(item), allocator);
        return *this;
    }

    UTILS_PROC Array push(const T &item,
                          Allocator *allocator = context_allocator_peek()) {
        insert(item, Base::length, allocator);
        return *this;
    }
    UTILS_PROC T &
    push_reserve(Allocator *allocator = context_allocator_peek()) {
        return insert_reserve(Base::length, allocator);
    }

    UTILS_PROC T pop(Allocator *allocator = context_allocator_peek()) {
        return remove(-1, allocator);
    }

    UTILS_PROC Array unshift(const T &item,
                             Allocator *allocator = context_allocator_peek()) {
        return insert(item, 0, allocator);
    }
    UTILS_PROC T &
    unshift_reserve(Allocator *allocator = context_allocator_peek()) {
        return insert_reserve(0, allocator);
    }

    UTILS_PROC T shift(Allocator *allocator = context_allocator_peek()) {
        return remove(0, allocator);
    }

    UTILS_PROC Maybe<T *> search(const T &value) {
        for (usize i = 0; i < Base::length; i++) {
            T *it = &Base::at(i);

            if (it == value) {
                return some(it);
            }
        }

        return none<T>();
    }
};

template <typename T> struct List {
    struct Node {
        T head = {};
        Node *tail = nullptr;
    };

    struct iterator {
        Node *current;

        UTILS_PROC_CONSTRUCT iterator(Node *ptr) : current(ptr) {}

        UTILS_PROC bool operator==(const iterator &other) const {
            return current == other.current;
        }

        UTILS_PROC bool operator!=(const iterator &other) const {
            return !(*this == other);
        }

        UTILS_PROC iterator &operator++() {
            current = current->tail;
            return *this;
        }

        UTILS_PROC iterator &operator++(T) {
            iterator result = *this;
            ++(*this);
            return result;
        }

        UTILS_PROC Node &operator*() { return *current; }
        UTILS_PROC Node *operator->() { return current; }
    };

    Node *node;

    UTILS_PROC_CONSTRUCT List() : node{nullptr} {}

    UTILS_PROC_CONSTRUCT List(Node *node) : node{node} {}

    UTILS_PROC List loop(const auto &f) const {
        Node *current = node;
        while (current != nullptr) {
            if (!f(current))
                break;
            current = current->tail;
        }

        return *this;
    }

    UTILS_PROC uint length() const {
        uint result = 0;
        loop([&](Node *) {
            result++;
            return true;
        });
        return result;
    }

    UTILS_PROC Node &last() const {
        Node *result = nullptr;
        loop([&](Node *current) {
            result = current;
            return true;
        });
        return *result;
    }

    UTILS_PROC Node &at(uint index) const {
        Node *current = node;

        for (uint i = 0; i < index; i++)
            current = current->tail;

        return *current;
    }

    UTILS_PROC List append(const T &item,
                           Allocator *allocator = context_allocator_peek()) {
        if (node == nullptr) {
            return prepend(item);
        }

        Node *current = &last();

        current->tail = alloc<Node>(1, allocator);
        current = current->tail;
        current->head = item;
        current->tail = nullptr;

        return *this;
    }

    UTILS_PROC List prepend(const T &item,
                            Allocator *allocator = context_allocator_peek()) {
        Node *result = alloc<Node>(1, allocator);
        result->head = item;
        result->tail = node;

        node = result;

        return *this;
    }

    UTILS_PROC List remove(uint index,
                           Allocator *allocator = context_allocator_peek()) {
        if (node == nullptr)
            return *this;

        Node *to_remove = &at(index);
        if (index == 0) {
            node = node->tail;
        } else if (index == length()) {
            at(index - 1).tail = nullptr;
        } else {
            at(index - 1).tail = &at(index + 1);
        }

        dealloc(to_remove, 1, allocator);
        return *this;
    }

    UTILS_PROC List forward() { return remove(0); }

    UTILS_PROC iterator begin() { return iterator(node); }
    UTILS_PROC iterator end() { return iterator(nullptr); }

    // https://cp-algorithms.com/others/tortoise_and_hare.html
    // TODO: bool has_cycle() {...} //
    // TODO: List chain() {...} // create a cycle
};

template <typename T> struct Tree {
    struct Node {
        T head = {};
        Node *next = nullptr;
        Node *prev = nullptr;
    };

    struct iterator {
        Node *current;

        UTILS_PROC_CONSTRUCT iterator(Node *ptr) : current(ptr) {}

        UTILS_PROC bool operator==(const iterator &other) const {
            return current == other.current;
        }

        UTILS_PROC bool operator!=(const iterator &other) const {
            return !(*this == other);
        }

        UTILS_PROC iterator &operator++() {
            current = current->next;
            return *this;
        }

        UTILS_PROC iterator &operator++(T) {
            iterator result = *this;
            ++(*this);
            return result;
        }

        UTILS_PROC iterator &operator--() {
            current = current->prev;
            return *this;
        }

        UTILS_PROC iterator &operator--(T) {
            iterator result = *this;
            --(*this);
            return result;
        }

        UTILS_PROC Node &operator*() { return *current; }
        UTILS_PROC Node *operator->() { return current; }
    };

    using iterator_type = iterator;
    using iterator_it_type = Node;

    Node *node;

    UTILS_PROC_CONSTRUCT Tree() : node{nullptr} {}

    UTILS_PROC_CONSTRUCT Tree(Node *node) : node{node} {}

    UTILS_PROC Tree loop_next(const auto &f) const {
        Node *current = node;
        while (current != nullptr) {
            if (!f(current))
                break;
            current = current->next;
        }

        return *this;
    }

    UTILS_PROC Tree loop_prev(const auto &f) const {
        Node *current = node;
        while (current != nullptr) {
            if (!f(current))
                break;
            current = current->prev;
        }

        return *this;
    }

    UTILS_PROC uint length_next() const {
        uint result = 0;

        if (node == nullptr)
            return 0;

        Node *current = node->next;
        while (current != nullptr) {
            current = current->next;
            result++;
        }

        return result;
    }

    UTILS_PROC uint length_prev() const {
        uint result = 0;

        if (node == nullptr)
            return 0;

        Node *current = node->prev;
        while (current != nullptr) {
            current = current->prev;
            result++;
        }

        return result;
    }

    UTILS_PROC uint length() const {
        if (node == nullptr)
            return 0;

        return 1 + length_next() + length_prev();
    }

    UTILS_PROC Node &last_next() const {
        Node *result = node;
        while (result->next != nullptr)
            result = result->next;
        return *result;
    }

    UTILS_PROC Node &last_prev() const {
        Node *result = node;
        while (result->prev != nullptr)
            result = result->prev;
        return *result;
    }

    UTILS_PROC Node &last() const {
        Node *left = node;
        Node *right = node;

        while (true) {
            if (left->prev == nullptr)
                return left;
            left = left->prev;

            if (right->next == nullptr)
                return right;
            right = right->next;
        }
    }

    UTILS_PROC Node &at_next(uint index) const {
        Node *current = node;

        for (uint i = 0; i < index; i++)
            current = current->next;

        return *current;
    }

    UTILS_PROC Node &at_prev(uint index) const {
        Node *current = node;

        for (uint i = 0; i < index; i++)
            current = current->prev;

        return *current;
    }

    UTILS_PROC Node &at(int index) const {
        if (index == 0)
            return *node;
        if (index > 0)
            return at_next((uint)index);
        if (index < 0)
            return at_prev((uint)(index * -1));

        UTILS_UNREACHEBLE();
    }

    UTILS_PROC Tree
    append_next(const T &item, Allocator *allocator = context_allocator_peek()) {
        if (node == nullptr)
            prepend_next(item);

        Node *last = &last_next();
        Node *it = alloc<Node>(1, allocator);

        last->next = it;

        it->head = item;
        it->next = nullptr;
        it->prev = last;

        return *this;
    }

    UTILS_PROC Tree
    append_prev(const T &item, Allocator *allocator = context_allocator_peek()) {
        if (node == nullptr)
            prepend_prev(item);

        Node *last = &last_prev();
        Node *it = alloc<Node>(1, allocator);

        last->prev = it;

        it->head = item;
        it->next = last;
        it->prev = nullptr;

        return *this;
    }

    UTILS_PROC Tree
    prepend_next(const T &item, Allocator *allocator = context_allocator_peek()) {
        Node *it = alloc<Node>(1, allocator);

        it->head = item;
        it->next = node;
        it->prev = non_nil(node)->prev;

        non_nil(non_nil(node)->prev)->next = it;
        non_nil(it->prev)->next = it;

        swap(node, it);

        return *this;
    }

    UTILS_PROC Tree
    prepend_prev(const T &item, Allocator *allocator = context_allocator_peek()) {
        Node *it = alloc<Node>(1, allocator);

        it->head = item;
        it->next = non_nil(node)->next;
        it->prev = node;

        non_nil(non_nil(node)->next)->prev = it;
        non_nil(node)->next = it;

        swap(node, it);

        return *this;
    }

    UTILS_PROC Node remove(int index = 0, int choose = 0,
                           Allocator *allocator = context_allocator_peek()) {
        UTILS_ASSERT(node != nullptr);

        Node *to_remove = &at(index);
        Node result = *to_remove;
        Node *it_prev = at(index).prev;
        Node *it_next = at(index).next;

        non_nil(it_prev)->next = it_next;
        non_nil(it_next)->prev = it_prev;

        if (index == 0)
            node = max(it_prev, it_next);

        if (choose > 0 && node != it_next && it_next != nullptr)
            node = it_next;

        if (choose < 0 && node != it_prev && it_prev != nullptr)
            node = it_prev;

        dealloc(to_remove, 1, allocator);
        return result;
    }

    UTILS_PROC Node forward() { return remove(0, 1); }
    UTILS_PROC Node backward() { return remove(0, -1); }

    UTILS_PROC iterator begin() { return iterator(&last_prev()); }
    UTILS_PROC iterator end() { return iterator(&last_next()); }
};

template <typename T> struct Set : private Array<T> {
    using Base = Array<T>;

    UTILS_PROC Set add(const T &item) {
        if (Base::binary_search(item))
            return *this;

        Base::sorted_insert(item);
        return *this;
    }

    UTILS_PROC Set del(const T &item) {
        if (!Base::binary_search(item))
            return *this;

        Base::sorted_remove(item);
        return *this;
    }

    UTILS_PROC bool in(const T &item) { return Base::binary_search(item).has; }

    UTILS_PROC Base::iterator begin() { return Base::begin(); }

    UTILS_PROC Base::iterator end() { return Base::end(); }

    UTILS_PROC Base to_array() { return *static_cast<Base*>(this); }
};

template <typename Key, typename Value>
struct Table : private Array<Pair<Key, Value>> {
    using Item = Pair<Key, Value>;
    using Base = Array<Item>;
    using Cmp = int (*)(const Key &a, const Key &b);
    using iterator_type = Base::iterator_type;
    using iterator_it_type = Base::iterator_it_type;

    struct Opts {
        Cmp cmp = Table::cmp;
        Allocator *allocator = context_allocator_peek();
    };

    UTILS_PROC static int cmp(const Key &a, const Key &b) {
        return a <=> b;
    }

    UTILS_PROC Base::iterator_type begin() { return Base::begin(); }

    UTILS_PROC Base::iterator_type end() { return Base::end(); }

    UTILS_PROC Table reserve(usize increment) {
        Base::reserve(increment);
        return *this;
    }

    UTILS_PROC Maybe<Item *> search(const Key &key, Cmp cmp = Table::cmp) const {
        usize low = 0, high = Base::length;

        while (low < high) {
            usize mid = (low + high) / 2;
            Item *it = &Base::at(mid);
            int c = cmp(it->key, key);

            if (c == 0)
                return some(it);

            if (c < 0)
                low = mid + 1;
            else
                high = mid;
        }

        return none<Item *>();
    }

    UTILS_PROC Table put(const Item &item, Opts opts = {}) {
        if (auto it = search(item.key, opts.cmp); it) {
            (*it)->value = item.value;
        }

        Base::sorted_insert(item, opts.allocator);
        return *this;
    }

    UTILS_PROC Table put(std::initializer_list<const Item> items, Opts opts = {}) {
        reserve(Base::length + items.size(), opts.allocator);

        for (const auto &item : items) {
            if (auto it = search(item.key, opts.cmp); it) {
                (*it)->value = item.value;
                continue;
            }

            Base::push(item, opts.allocator);
        }

        Base::sort(opts.cmp);
        return *this;
    }

    UTILS_PROC Maybe<Value> get(const Key &key, Cmp cmp = Table::cmp) const {
        if (auto it = search(key, cmp); it) {
            return some((*it)->value);
        }

        return none<Value>();
    }

    UTILS_PROC Value &operator[](const Key &key, Opts opts = {}) {
        if (auto it = search(key, opts.cmp); it) {
            return (*it)->value;
        }

        Item *it = &Base::sorted_reserve(Item(key, {}), opts.allocator);
        it->key = key;
        it->value = {};
        return it->value;
    }

    UTILS_PROC Base to_array() { return *static_cast<Base *>(this); }
};

struct String : public Slice<char> {
    using Base = Slice<char>;
    UTILS_PROC_CONSTRUCT String() : Slice() {}
    UTILS_PROC_CONSTRUCT String(Slice slice) : Slice(slice) {}

    UTILS_PROC_CONSTRUCT String(char *chars, usize length)
        : Slice(chars, length) {}

    UTILS_PROC bool is_zstr() const {
        return Base::at(Base::length + 1) == '\0';
    }

    UTILS_PROC char *temp_zstr() const {
        static char result[1024] = {0};

        UTILS_ASSERT(Base::length <= sizeof(result));
        memset(result, 0, sizeof(result));
        copy(Base::items, result, Base::length);

        return result;
    };
};

struct String_Builder {
    Array<char> chars;
    Array<usize> control;

    UTILS_PROC String_Builder
    write(char c, Allocator *allocator = context_allocator_peek()) {
        chars.push(c, allocator);
        return *this;
    }

    UTILS_PROC String_Builder write(
        const String &string, Allocator *allocator = context_allocator_peek()) {
        chars.reserve(chars.length + string.length, allocator);
        copy(string.items, &chars.at_raw(chars.length), string.length);
        chars.length += string.length;

        return *this;
    }

    UTILS_PROC char *start(Allocator *allocator = context_allocator_peek()) {
        usize index = chars.length;
        control.push(index, allocator);
        return &chars.items[index];
    }

    UTILS_PROC String end(Allocator *allocator = context_allocator_peek()) {
        usize offset = control.pop(allocator);
        return String(chars.splice(offset, -1));
    }

    UTILS_PROC String_Builder
    reset(Allocator *allocator = context_allocator_peek()) {
        dealloc<char>(chars.items, chars.length, allocator);
        chars.items = nullptr;
        chars.length = 0;
        chars.capacity = 0;

        dealloc<usize>(control.items, control.length, allocator);
        control.items = nullptr;
        control.length = 0;
        control.capacity = 0;

        return *this;
    }
};

template <usize N> struct Allocator_Static : public Allocator {
    Slice<u8> pool;
    usize used;

    UTILS_PROC_CONSTRUCT Allocator_Static(Slice<u8> pool)
        : pool(pool), used(0) {
        UTILS_ASSERT(pool.length == N);
    }

    UTILS_PROC_CONSTRUCT Allocator_Static()
        : pool(Slice<u8>(reinterpret_cast<u8 *>(std::malloc(N)), N)), used(0) {}

    UTILS_PROC void *run(void *ptr, usize old_size, usize new_size) override {
        UTILS_ASSERT((used + new_size) <= N);

        // dealloc
        if (new_size == 0) {
            return nullptr;
        }

        // alloc
        void *result = &pool[used];
        used += new_size;

        // realloc
        if (ptr != nullptr && old_size != 0)
            memcpy(result, ptr, old_size);

        return result;
    }

    UTILS_PROC void destroy() override {
        if (pool.items == nullptr)
            return;

        std::free(pool.items);
    }

    UTILS_PROC void reset() override { used = 0; }
};

template <usize N> struct Allocator_Ring : public Allocator {
    Slice<u8> pool;
    usize used;

    UTILS_PROC_CONSTRUCT Allocator_Ring(Slice<u8> pool) : pool(pool), used(0) {
        UTILS_ASSERT(pool.length == N);
    }

    UTILS_PROC_CONSTRUCT Allocator_Ring()
        : pool{Slice<u8>(reinterpret_cast<u8 *>(std::malloc(N)), N)}, used{0} {}

    UTILS_PROC void *run(void *ptr, usize old_size, usize new_size) override {
        if ((used + new_size) > N) {
            used = 0;
        }

        // dealloc
        if (new_size == 0) {
            return nullptr;
        }

        // alloc
        void *result = &pool[used];
        used += new_size;

        // realloc
        if (ptr != nullptr && old_size != 0)
            memcpy(result, ptr, old_size);

        return result;
    }

    UTILS_PROC void destroy() override {
        if (pool.items == nullptr)
            return;

        std::free(pool.items);
    }

    UTILS_PROC void reset() override { used = 0; }
};

struct Allocator_Malloc : public Allocator {
    void **ptrs;
    usize length;
    usize capacity;

    static constexpr usize init_capacity = 1 << 10;

    UTILS_PROC void *run(void *ptr, usize, usize new_size) override {
        // init
        if (ptrs == NULL) {
            capacity = init_capacity;
            ptrs = reinterpret_cast<void **>(
                std::malloc(sizeof(*ptrs) * capacity));
            length = 0;
        }

        // free
        if (new_size == 0) {
            // search(ptr)
            for (uint i = 0; i < length; i++) {
                void *check_ptr = ptrs[i];
                if (check_ptr != ptr)
                    continue;

                std::free(check_ptr);

                // remove(i)
                Slice<void *>(ptrs, length).splice(i, length).rotate(-2);
                length--;
            }

            return nullptr;
        }

        void *result =
            std::realloc(ptr, /* memory alignment */ max(new_size, 16));
        UTILS_ASSERT(result != nullptr);

        // alloc
        if (ptr == NULL) {
            // push(result)
            length++;
            if (length >= capacity) {
                capacity *= 2;
                void **p = reinterpret_cast<void **>(
                    std::realloc(ptrs, sizeof(*ptrs) * capacity));
                UTILS_ASSERT(p != NULL);
                ptrs = p;
            }
            ptrs[length - 1] = result;
        } else /* relloac */ {
            // replace(ptr, result)
            for (uint i = 0; i < length; i++) {
                if (ptrs[i] == ptr) {
                    ptrs[i] = result;
                    break;
                }
            }
        }

        return result;
    }

    UTILS_PROC void destroy() override {
        if (ptrs == nullptr)
            return;

        for (usize i = 0; i < length; i++) {
            std::free(ptrs[i]);
        }

        std::free(ptrs);
        ptrs = nullptr;
        length = 0;
        capacity = 0;
    }

    UTILS_PROC void reset() override { length = 0; }
};

template <typename T> struct Unique {
    T *ptr;
    usize ptr_count;

    UTILS_PROC_CONSTRUCT Unique(T *ptr, usize ptr_count)
        : ptr(ptr), ptr_count(ptr_count) {}

    UTILS_PROC_CONSTRUCT_COPY Unique(const Unique &other) = delete;

    UTILS_PROC_CONSTRUCT_MOVE Unique(Unique &&other)
        : ptr{other.ptr}, ptr_count{other.ptr_count} {
        other.ptr = nullptr;
        other.ptr_count = 0;
    }

    UTILS_PROC Unique &operator=(const Unique &other) = delete;

    UTILS_PROC Unique &operator=(const Unique &&other) {
        ptr = other.ptr;
        ptr_count = other.ptr_count;

        return *this;
    }

    UTILS_PROC T *addr() { return ptr; }
    UTILS_PROC T operator*() { return *addr(); }
    UTILS_PROC T *operator->() { return addr(); }

    UTILS_PROC void destroy(Allocator *allocator = context_allocator_peek()) {
        if (ptr == nullptr || ptr_count == 0)
            return;

        dealloc(ptr, ptr_count, allocator);

        ptr = nullptr;
        ptr_count = 0;
    }

    UTILS_PROC_DESTRUCT ~Unique() { destroy(); }
};

template <typename T> struct Shared {
    struct Control_Block {
        usize ptr_count;
        uint *counter;

        UTILS_PROC_CONSTRUCT Control_Block(usize ptr_count, uint *counter)
            : ptr_count(ptr_count), counter(counter) {}

        UTILS_PROC_CONSTRUCT
        Control_Block(usize ptr_count, Allocator *allocator = context_allocator_peek())
            : ptr_count(ptr_count), counter(alloc<uint>(1, allocator)) {
            *counter = 1;
        }

        UTILS_PROC uint inc() { return ++(*counter); }
        UTILS_PROC uint dec() { return --(*counter); }
    };

    T *ptr = nullptr;
    Control_Block *control = nullptr;

    UTILS_PROC_CONSTRUCT Shared(T *ptr,
                                usize ptr_count,
                                Allocator *allocator = context_allocator_peek())
        : ptr(ptr), control(alloc<Control_Block>(1, allocator)) {
        *control = Control_Block(ptr_count);
    }

    UTILS_PROC_CONSTRUCT_COPY
    Shared(Shared &other, Allocator *allocator = context_allocator_peek())
        : ptr(other.ptr), control(alloc<Control_Block>(1, allocator)) {
        *control = Control_Block(other.control->ptr_count, other.control->counter);
        other.control->inc();
    }

    UTILS_PROC_CONSTRUCT_MOVE Shared(Shared &&other)
        : ptr(other.ptr), control(Control_Block(other.control->ptr_count,
                                other.control->count)) {
        other.control->destroy();
    }

    UTILS_PROC Shared &operator=(Shared &other) {
        control->destroy();

        other.control->inc();

        control->ptr = control->other.ptr;
        control->ptr_count = control->other.ptr_count;
        control->count = control->other.count;

        return *this;
    }

    UTILS_PROC Shared &operator=(Shared &&other) {
        control->destroy();

        control->ptr = control->other.ptr;
        control->ptr_count = control->other.ptr_count;
        control->count = control->other.count;

        other.control->destroy();

        return *this;
    }


    UTILS_PROC void destroy(Allocator *allocator = context_allocator_peek()) {
        if (ptr == nullptr || control->ptr_count == 0 || control->counter == nullptr ||
            *control->counter == 0)
            return;

        if (control->dec() == 0) {
            dealloc<T>(ptr, 1, allocator);
            ptr = nullptr;
            control->ptr_count = 0;

            dealloc<uint>(control->counter, 1, allocator);
            control->counter = nullptr;

            dealloc<Control_Block>(control, 1, allocator);
        }
    }


    UTILS_PROC T *addr() { return ptr; }
    UTILS_PROC T operator*() { return *addr(); }
    UTILS_PROC T *operator->() { return addr(); }

    UTILS_PROC uint counter() { return *control->counter; }

    UTILS_PROC_DESTRUCT ~Shared() { destroy(); }
};

UTILS_PROC String operator""_s(const char *chars, usize length);

UTILS_PROC String string_from_zstr(char *zstr);
UTILS_PROC String file_read(const String &file_path, String_Builder &sb);
} // namespace utils

#ifdef UTILS_IMPLEMENT
namespace utils {

Allocator *context_allocators[UTILS_MAX_CONTEXT_ALLOCATORS] = {0};
usize context_allocators_count{};

u64 random_state = 1ULL << 31 | 1;

UTILS_PROC bool bitf_get(const u8 *n, u8 index) { return *n & (1 << index); }
UTILS_PROC void bitf_set(u8 *n, u8 index, bool bit) {
    if (bit)
        bitf_on(n, index);
    else
        bitf_off(n, index);
}
UTILS_PROC void bitf_on(u8 *n, u8 index) { *n |= (1 << index); }
UTILS_PROC void bitf_off(u8 *n, u8 index) { *n &= ~(1 << index); }
UTILS_PROC void bitf_toggle(u8 *n, u8 index) { *n ^= (1 << index); }

UTILS_PROC f32 random_norm() {
    u32 full = 0;
    usize full_size = sizeof(full) * 8;

    for (uint i = 0; i < full_size; i++) {
        random_state ^= random_state >> 7;
        random_state ^= random_state << 9;
        random_state ^= random_state >> 13;

        full = full | (random_state & 1) << i;
    }

    return ((float)full / ~(1L << full_size)) * -1;
}

UTILS_PROC ssize ptr_diff(void *a, void *b) {
    return reinterpret_cast<ssize>(a) - reinterpret_cast<ssize>(b);
}

UTILS_PROC String file_read(const String &path, String_Builder &sb) {
    FILE *file = fopen(path.temp_zstr(), "rb+");
    UTILS_DEFER[file] { fclose(file); };

    long start = ftell(file);
    assert(start >= 0);

    fseek(file, 0, SEEK_END);
    long end = ftell(file);
    assert(end >= 0);

    assert(end >= start);
    fseek(file, start, SEEK_SET);

    long read_size = end - start;
    if (read_size <= 0)
        return String();

    sb.chars.reserve(sb.chars.length + read_size);
    char *s = sb.start();
    assert(fread(s, 1, read_size, file) == read_size);
    sb.chars.length += read_size;

    return sb.end();
}

UTILS_PROC String operator""_s(const char *chars, usize length) {
    return String(const_cast<char *>(chars), length);
}

UTILS_PROC String string_from_zstr(char *zstr) {
    return String(zstr, strlen(zstr));
}

UTILS_PROC char *zstr_from_string(const String &string, String_Builder sb) {
    char *result = sb.start();
    sb.write(string);
    sb.write('\0');
    sb.end();
    return result;
}

UTILS_PROC void context_allocator_push(Allocator *allocator) {
    assert(context_allocators_count <= UTILS_MAX_CONTEXT_ALLOCATORS);

    context_allocators[context_allocators_count++] = allocator;
}

UTILS_PROC Allocator *context_allocator_replace(Allocator *allocator) {
    Allocator *result = context_allocators[context_allocators_count - 1];
    context_allocators[context_allocators_count - 1] = allocator;
    return result;
}

UTILS_PROC Allocator *context_allocator_pop() {
    assert(context_allocators_count > 0);

    return context_allocators[--context_allocators_count];
}

UTILS_PROC Allocator *context_allocator_peek() {
    assert(context_allocators_count > 0);

    return context_allocators[context_allocators_count - 1];
}

UTILS_PROC void context_allocator_destroy() {
    context_allocator_pop()->destroy();
}

UTILS_PROC void context_allocator_reset() { context_allocator_peek()->reset(); }

UTILS_PROC_INIT void _context_allocator_init() {
    static auto allocator = Allocator_Malloc();

    context_allocator_push(&allocator);
}

UTILS_PROC_FINI void _context_allocator_fini() {
    while (context_allocators_count > 0)
        context_allocator_pop()->destroy();
}
} // namespace utils
#endif
