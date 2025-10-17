#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <utility>

#ifndef UTILS_PROC
#define UTILS_PROC inline
#endif

#ifndef UTILS_ASSERT
#include <assert.h>
#define UTILS_ASSERT assert
#endif

#ifndef UTILS_BREAKPOINT
#define UTILS_BREAKPOINT() __asm__("int3")
#endif

#ifndef UTILS_PROC_INIT
#define UTILS_PROC_INIT __attribute__((constructor))
#endif

#ifndef UTILS_PROC_FINI
#define UTILS_PROC_FINI __attribute__((destructor))
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

#define VA_ARGS_HEAD(HEAD, ...) HEAD
#define VA_ARGS_TAIL(_HEAD, ...) __VA_ARGS__

#define _CONCAT(X, Y) X##Y
#define CONCAT(X, Y) _CONCAT((X), (Y))

#define _STRINGFY(X) #X
#define STRINGFY(X) _STRINGFY((X))

#define ARRAY_LENGTH(X) (sizeof((X)) / sizeof((X)[0]))
#define BIG_NULL ((Big_Ptr){0})

UTILS_PROC ssize ptr_diff(void *a, void *b) {
    return reinterpret_cast<ssize>(a) - reinterpret_cast<ssize>(b);
}

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

template <typename T> UTILS_PROC T clamp(T x, T low, T high) {
    return min(max(low, x), high);
}

template <typename T> UTILS_PROC T mod(T a, T b) { return (a % b + b) % b; }

template <typename T> UTILS_PROC void swap(T &a, T &b) {
    T tmp = a;
    a = b;
    b = tmp;
}

template <typename T> UTILS_PROC void copy(T *src, T *dest) {
    memcpy(static_cast<T *>(dest), static_cast<T *>(src), sizeof(T));
}

template <typename T> struct Maybe {
    T value;
    bool has;
};

template <typename T> Maybe<T> UTILS_PROC some(T value) {
    return (Maybe<T>){
        .value = value,
        .has = true,
    };
}

template <typename T> Maybe<T> UTILS_PROC none() {
    return (Maybe<T>){
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
Result<Value, Error> result_value(Value value);

template <typename Value, typename Error>
Result<Value, Error> result_error(Error value);

struct Allocator {
    virtual void *run(void *ptr, usize old_size, usize new_size) = 0;
    virtual void destroy() = 0;
    virtual void reset() = 0;
};

Allocator *global_allocator = nullptr;

template <typename T> struct Slice {
    T *items;
    usize length;

    UTILS_PROC Slice() : items{nullptr}, length{0} {}

    UTILS_PROC Slice(T *items, usize length) : items{items}, length{length} {}

    // UTILS_PROC Slice(std::initializer_list<T> values)
    //     : items{(T *)(values.begin())}, length{values.size()} {}

    UTILS_PROC T &at(ssize index) const {
        if (index < 0)
            index += length;

        // UTILS_ASSERT(index >= 0);
        //  UTILS_ASSERT(index < length);
        UTILS_ASSERT(items != nullptr);
        return items[index];
    }

    UTILS_PROC T &operator[](usize index) const { return at(index); }

    UTILS_PROC Slice<T> splice(ssize start, ssize end) const {
        // UTILS_ASSERT(mod<ssize>(end, (length + 1)) >
        //              mod<ssize>(start, (length + 1)));
        // UTILS_ASSERT(start <= length);
        // UTILS_ASSERT(end <= length);

        return Slice<T>(&at(start), end - start);
    }

    UTILS_PROC void swap(ssize a, ssize b) {
        if (a == b)
            return;

        utils::swap(at(a), at(b));
    }

    UTILS_PROC Slice<T> reverse() {
        for (uint i = 0; i < length / 2; i++) {
            swap(i, length - 1 - i);
        }

        return *this;
    }

    UTILS_PROC Slice<T> rotate(ssize n) {
        reverse();
        splice(0, n).reverse();
        splice(n, -1).reverse();
        return *this;
    }
};

template <typename T> struct Array {
    union {
        struct {
            T *items;
            usize length;
        };
        Slice<T> slice;
    };
    usize capacity;

    UTILS_PROC Array() : items{nullptr}, length{0}, capacity{0} {}

    UTILS_PROC Array(T *items, usize length)
        : items{items}, length{0}, capacity{length} {}

    // UTILS_PROC Array(std::initializer_list<T> values)
    //     : items{const_cast<T *>(values.begin())}, length{values.size()} {}

    UTILS_PROC T &at(ssize index) const { return slice.at(index); }
    UTILS_PROC T &operator[](usize index) const { return at(index); }

    UTILS_PROC void rotate(ssize n) { slice.rotate(n); }

    UTILS_PROC Slice<T> splice(ssize start, ssize end) const {
        return slice.splice(start, end);
    }

    UTILS_PROC void reserve(usize increment,
                            Allocator *allocator = global_allocator) {
        if (items == NULL) {
            capacity = increment;
            items = alloc<T>(capacity, allocator);
            return;
        }

        usize old_size = capacity;
        capacity += increment;
        usize new_size = capacity;
        items = realloc<T>(items, old_size, new_size, allocator);
    }

    UTILS_PROC void insert(T item, usize index,
                           Allocator *allocator = global_allocator) {
        UTILS_ASSERT(index <= length);

        if (capacity == 0) {
            reserve(1, allocator);
        } else if (length >= capacity) {
            reserve(capacity, allocator);
        }

        length += 1;
        at(-1) = item;

        if ((length - 1) != index) {
            splice(index, length).rotate(1);
        }
    }

    UTILS_PROC void push(T item, Allocator *allocator = global_allocator) {
        insert(item, length, allocator);
    }
};

template <typename T> struct List {
    T *head;
    List *tail;
};

template <typename T> struct Double_List {
    T *head;
    Double_List *next;
    Double_List *prev;
};

struct String {
    char *chars;
    usize length;
};

union String_Builder {
    struct {
        union {
            struct {
                char *chars;
                usize length;
            };
            String string;
        };
        usize capacity;
    };
    Array<char> array;
};

template <usize N> struct Allocator_Static : Allocator {
    Slice<u8> pool;
    usize used;

    Allocator_Static()
        : pool{Slice<u8>(reinterpret_cast<u8 *>(std::malloc(N)), N)}, used{0} {}

    UTILS_PROC void *run(void *ptr, usize old_size, usize new_size) override {
        UTILS_ASSERT((used + new_size) <= N);

        // dealloc
        if (new_size == 0) {
            return nullptr;
        }

        // allocxs
        void *result = &pool[used];
        used += new_size;

        // realloc
        if (ptr != nullptr && old_size != 0)
            std::memcpy(result, ptr, old_size);

        return result;
    }

    UTILS_PROC void destroy() override {
        if (pool.items == nullptr)
            return;

        std::free(pool.items);
    }

    UTILS_PROC void reset() override { used = 0; }
};

template <typename T>
UTILS_PROC T *alloc(uint count = 1, Allocator *allocator = global_allocator) {
    UTILS_ASSERT(allocator != nullptr);
    return static_cast<T *>(allocator->run(nullptr, 0, sizeof(T) * count));
}

template <typename T>
UTILS_PROC void dealloc(T *ptr, uint count,
                        Allocator *allocator = global_allocator) {
    UTILS_ASSERT(allocator != nullptr);
    allocator->run(ptr, sizeof(T) * count, 0);
}

template <typename T>
UTILS_PROC T *realloc(T *ptr, uint old_count, uint new_count,
                      Allocator *allocator = global_allocator) {
    UTILS_ASSERT(allocator != nullptr);
    return static_cast<T *>(
        allocator->run(ptr, sizeof(T) * old_count, sizeof(T) * new_count));
}

template <typename T> struct Unique {
    T *ptr;
    usize ptr_count;
    Allocator *allocator;

    UTILS_PROC Unique(T *ptr, usize ptr_count,
                      Allocator *allocator = global_allocator)
        : ptr{ptr}, ptr_count{ptr_count}, allocator{allocator} {}

    UTILS_PROC Unique(const Unique &other) = delete;

    UTILS_PROC Unique(Unique &&other)
        : ptr{other.ptr}, ptr_count{other.ptr_count},
          allocator{other.allocator} {
        other.destroy();
    }

    UTILS_PROC Unique &operator=(const Unique &other) = delete;

    UTILS_PROC Unique &operator=(const Unique &&other) {
        dealloc(ptr, allocator);

        ptr = other.ptr;
        ptr_count = other.ptr_count;
        allocator = other.allocator;

        return *this;
    }

    UTILS_PROC void destroy() {
        if (ptr == nullptr || ptr_count == 0)
            return;

        dealloc(ptr, ptr_count, allocator);

        ptr = nullptr;
        ptr_count = 0;
        allocator = nullptr;
    }

    UTILS_PROC ~Unique() { destroy(); }
};

template <typename T> struct Shared {
    T *ptr;
    usize ptr_count;
    Allocator *allocator;
    usize *count;

    UTILS_PROC Shared(T *ptr, usize ptr_count,
                      Allocator *allocator = global_allocator)
        : ptr{ptr}, ptr_count{ptr_count}, allocator{allocator},
          count{alloc<usize>(1, allocator)} {
        *count = 1;
    }

    UTILS_PROC Shared(Shared &other)
        : ptr{other.ptr}, allocator{other.allocator}, count{other.count} {
        (*other.count)++;
    }

    UTILS_PROC Shared(Shared &&other)
        : ptr{ptr}, allocator{allocator}, count{count} {
        other.destroy();
    }

    UTILS_PROC Shared &operator=(Shared &other) {
        destroy();

        other.count++;

        ptr = other.ptr;
        ptr_count = other.ptr_count;
        allocator = other.allocator;
        count = other.count;

        return *this;
    }

    UTILS_PROC Shared &operator=(Shared &&other) {
        destroy();

        ptr = other.ptr;
        ptr_count = other.ptr_count;
        allocator = other.allocator;
        count = other.count;

        return *this;
    }

    UTILS_PROC void destroy() {
        if (ptr == nullptr || ptr_count == 0 || count == nullptr)
            return;

        (*count)--;

        if (*count != 0)
            return;

        dealloc(ptr, ptr_count, allocator);
        dealloc(count, 1, allocator);
    }

    UTILS_PROC ~Shared() { destroy(); }
};

UTILS_PROC_INIT void _global_allocator_destroy_init() {
    static auto _allocator = Allocator_Static<1024 * 8>();
    global_allocator = &_allocator;
}

UTILS_PROC_FINI void _global_allocator_destroy_fini() {
    global_allocator->destroy();
}

} // namespace utils
