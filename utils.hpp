#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

#include <cstring>

#include <initializer_list>

#ifndef UTILS_PROC
#define UTILS_PROC inline constexpr
#endif

#ifndef UTILS_ASSERT
#include <assert.h>
#define UTILS_ASSERT assert
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

template <typename T> struct Slice {
    T *items;
    usize length;

    UTILS_PROC Slice() : items{nullptr}, length{0} {}

    UTILS_PROC Slice(T *items, usize length) : items{items}, length{length} {}

    UTILS_PROC Slice(std::initializer_list<T> values)
        : items{(T *)(values.begin())}, length{values.size()} {}

    UTILS_PROC T &at(ssize index) const {
        if (index < 0)
            index += length;

        UTILS_ASSERT(index >= 0);
        // UTILS_ASSERT(index < length);
        UTILS_ASSERT(items != nullptr);
        return items[index];
    }

    UTILS_PROC T &operator[](usize index) const { return at(index); }

    UTILS_PROC Slice<T> &splice(ssize start, ssize end) const {
        UTILS_ASSERT(mod<ssize>(end, length) > mod<ssize>(start, length));
        UTILS_ASSERT(start < length);
        UTILS_ASSERT(end < length);

        return Slice<T>(at(start), end - start);
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

    UTILS_PROC Array(std::initializer_list<T> values)
        : items{const_cast<T *>(values.begin())}, length{values.size()} {}

    UTILS_PROC T &at(ssize index) const { return slice.at(index); }
    UTILS_PROC T &operator[](usize index) const { return at(index); }
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

    Allocator_Static() {
        pool = Slice<u8>(new u8[N], N);
        used = 0;
    }

    UTILS_PROC void *run(void *ptr, usize old_size, usize new_size) override {
        // dealloc
        if (new_size == 0)
            return nullptr;

        // alloc
        void *result = &pool.at(used);
        used += new_size;

        // realloc
        if (ptr != nullptr && old_size != 0)
            std::memcpy(result, ptr, old_size);

        return result;
    }

    UTILS_PROC void destroy() override { delete[] pool.items; }
    UTILS_PROC void reset() override { used = 0; }
};

struct Context {
    Allocator *allocator;
    // logger
    // dynamic library pointer
    // stdout, stdin, stderr

    Context(Allocator *allocator) : allocator{allocator} {}
};

// my own dynamic scope
struct Environment {
    static Environment *instance;

    Array<Context> contexts;
    Allocator_Static<1024 * 8> fallback;

    // Environment() {};
    Environment(Environment &) = delete;
    void operator=(const Environment &) = delete;
};

static Context *context();

template <typename T>
UTILS_PROC T *alloc(uint count = 1,
                    Allocator *allocator = context()->allocator) {
    // return static_cast<T *>(allocator->run(nullptr, 0, sizeof(T) * count));
    return (T *)(allocator->run(nullptr, 0, sizeof(T) * count));
}

template <typename T>
UTILS_PROC void dealloc(T *ptr, uint count,
                        Allocator *allocator = context()->allocator) {
    allocator->run(ptr, sizeof(T) * count, 0);
}

template <typename T>
UTILS_PROC T *realloc(T *ptr, uint old_count, uint new_count,
                      Allocator *allocator = context()->allocator) {
    return static_cast<T *>(
        allocator->run(ptr, sizeof(T) * old_count, sizeof(T) * new_count));
}

static Context *context() {
    if (Environment::instance == nullptr) {
        static u8 instance_memory[sizeof(Environment)] = {0};
        Environment::instance =
            reinterpret_cast<Environment *>(instance_memory);

        Environment::instance->fallback = Allocator_Static<1024 * 8>();

        Environment::instance->contexts =
            Array<Context>((Context *)Environment::instance->fallback.run(
                               nullptr, 0, sizeof(Context)),
                           1);

        Environment::instance->contexts[0] =
            Context(new Allocator_Static<1024 * 8>());
        Environment::instance->contexts.length = 1;
    }

    return &Environment::instance->contexts[-1];
}

Environment *Environment::instance = nullptr;

} // namespace utils
