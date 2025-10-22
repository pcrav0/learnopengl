#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <utility>

#define UTILS_IGNORE(...)

#ifndef UTILS_PROC
#define UTILS_PROC constexpr inline
#endif

#ifndef UTILS_PROC_INIT
#define UTILS_PROC_INIT UTILS_PROC __attribute__((constructor))
#endif

#ifndef UTILS_PROC_FINI
#define UTILS_PROC_FINI UTILS_PROC __attribute__((destructor))
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
#define UTILS_PROC_DESTRUCT UTILS_PROC
#endif

#ifndef UTILS_ASSERT
#include <assert.h>
#define UTILS_ASSERT(...)                                                      \
    do {                                                                       \
        assert(__VA_ARGS__);                                                   \
    } while (0)
#endif

#ifndef UTILS_BREAKPOINT
#define UTILS_BREAKPOINT() __asm__("int3")
#endif

#ifndef UTILS_NO_GLOBAL_ALLOCATOR
#define UTILS_GLOBAL_ALLOCATOR(VAR) VAR = global_allocator
#endif

#ifndef UTILS_UNREACHEBLE
#define UTILS_UNREACHEBLE() assert(false)
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

#define UTILS_VA_ARGS_HEAD(HEAD, ...) HEAD
#define UTILS_VA_ARGS_TAIL(_HEAD, ...) __VA_ARGS__

#define UTILS__CONCAT(X, Y) X##Y
#define UTILS_CONCAT(X, Y) UTILS__CONCAT(X, Y)

#define UTILS__STRINGFY(X) #X
#define UTILS_STRINGFY(X) UTILS__STRINGFY(X)

#define UTILS_ARRAY_LENGTH(X) (sizeof((X)) / sizeof((X)[0]))

template <typename F> struct _Defer {
    F f;
    _Defer(F f) : f(f) {}
    ~_Defer() { f(); }
};

#define UTILS_DEFER                                                            \
    utils::_Defer UTILS_CONCAT(UTILS_CONCAT(__defer__, __COUNTER__), __LINE__) =

#ifdef UTILS_MACRO_RENAME
#define VA_ARGS_HEAD UTILS_VA_ARGS_HEAD
#define VA_ARGS_TAIL UTILS_VA_ARGS_TAIL
#define CONCAT UTILS_CONCAT
#define STRINGFY UTILS_STRINGFY
#define ARRAY_LENGTH UTILS_ARRAY_LENGTH
#define DEFER UTILS_DEFER
#endif

UTILS_PROC ssize ptr_diff(void *a, void *b);

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

template <typename T> UTILS_PROC void copy(T *src, T *dest) {
    memcpy(static_cast<T *>(dest), static_cast<T *>(src), sizeof(T));
}

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
    union {
        // T first;
        T key;
        // T a;
        // T x;
    };
    union {
        // U second;
        U value;
        // U b;
        // U y;
    };

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
UTILS_PROC Result<Value, Error> result_value(Value value);

template <typename Value, typename Error>
UTILS_PROC Result<Value, Error> result_error(Error value);

struct Allocator {
    UTILS_PROC virtual void *run(void *ptr, usize old_size, usize new_size) = 0;
    UTILS_PROC virtual void destroy() = 0;
    UTILS_PROC virtual void reset() = 0;
};

#ifndef UTILS_NO_GLOBAL_ALLOCATOR
static Allocator *global_allocator = nullptr;
#endif

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

    UTILS_PROC_CONSTRUCT Slice() : items{nullptr}, length{0} {}

    UTILS_PROC_CONSTRUCT Slice(T *items, usize length)
        : items{items}, length{length} {}

    // UTILS_PROC Slice(std::initializer_list<T> values)
    //     : items{(T *)(values.begin())}, length{values.size()} {}

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
            int cmp = at(i) <=> at(i + 1);
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
};

template <typename T, Allocator *allocator = nullptr>
struct Array : public Slice<T> {
    usize capacity;

    static constexpr usize init_capacity = 256;

    UTILS_PROC_CONSTRUCT Array() : Slice<T>(), capacity{0} {}

    UTILS_PROC_CONSTRUCT Array(T *items, usize length)
        : Slice<T>(items, length), capacity{length} {}

    // UTILS_PROC Array(std::initializer_list<T> values)
    //     : items{const_cast<T *>(values.begin())}, length{values.size()} {}

    UTILS_PROC Slice<T> slice() const {
        return Slice<T>(this->items, this->length);
    }

    UTILS_PROC Array reserve(usize increment) {
        if (increment == 0)
            return *this;

        if (this->items == NULL) {
            capacity = increment;
            this->items = alloc<T>(capacity, allocator);
            return *this;
        }

        if (this->length + increment <= capacity)
            return *this;

        usize old_size = capacity;
        do {
            capacity *= 2;
        } while (capacity < this->length + increment);
        usize new_size = capacity;
        this->items = realloc<T>(this->items, old_size, new_size, allocator);
        return *this;
    }

    UTILS_PROC T &insert_reserve(usize index) {
        UTILS_ASSERT(index <= this->length);
        reserve(capacity == 0 ? init_capacity
                              : max(this->length, capacity * 2));

        this->length++;
        memset(&this->at(-1), 0, sizeof(T));

        if ((this->length - 1) != index)
            this->splice(index, this->length).rotate(1);

        return this->at_raw(index);
    }

    UTILS_PROC Array insert(T item, usize index) {
        insert_reserve(index) = item;
        return *this;
    }

    UTILS_PROC T remove(ssize index) {
        T result = this->at(index);

        if (index < 0)
            index += this->length + 1;

        if (index != this->length)
            this->splice(index, this->length).rotate(-2);
        this->length--;
        return result;
    }

    UTILS_PROC T &sorted_reserve(T item) {
        return insert_reserve(this->sorted_index(item));
    }

    UTILS_PROC Array sorted_insert(T item) {
        sorted_reserve(item) = item;
        return *this;
    }

    UTILS_PROC Array push(T item) {
        insert(item, this->length);
        return *this;
    }
    UTILS_PROC T &push_reserve() { return insert_reserve(this->length); }

    UTILS_PROC T pop() { return remove(-1); }

    UTILS_PROC Array unshift(T item) { return insert(item, 0); }
    UTILS_PROC T &unshift_reserve() { return insert_reserve(0); }

    UTILS_PROC T shift() { return remove(0); }

    UTILS_PROC Maybe<T *> search(T value) {
        for (usize i = 0; i < this->length; i++) {
            T *it = &this->at(i);

            if (it == value) {
                return some(it);
            }
        }

        return none<T>();
    }
};

template <typename T, Allocator *allocator = nullptr> struct List {
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

    UTILS_PROC List loop(auto f) const {
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

    UTILS_PROC List append(T item) {
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

    UTILS_PROC List prepend(T item) {
        Node *result = alloc<Node>(1, allocator);
        result->head = item;
        result->tail = node;

        node = result;

        return *this;
    }

    UTILS_PROC List remove(uint index) {
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

    // TODO: bool has_cycle() {...}
    // TODO: List chain() {...} // create a cycle
};

template <typename T, Allocator *allocator = nullptr> struct Double_List {
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

    UTILS_PROC_CONSTRUCT Double_List() : node{nullptr} {}

    UTILS_PROC_CONSTRUCT Double_List(Node *node) : node{node} {}

    UTILS_PROC Double_List loop_next(auto f) const {
        Node *current = node;
        while (current != nullptr) {
            if (!f(current))
                break;
            current = current->next;
        }

        return *this;
    }

    UTILS_PROC Double_List loop_prev(auto f) const {
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

    UTILS_PROC Double_List append_next(T item) {
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

    UTILS_PROC Double_List append_prev(T item) {
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

    UTILS_PROC Double_List prepend_next(T item) {
        Node *it = alloc<Node>(1, allocator);

        it->head = item;
        it->next = node;
        it->prev = non_nil(node)->prev;

        non_nil(non_nil(node)->prev)->next = it;
        non_nil(it->prev)->next = it;

        swap(node, it);

        return *this;
    }

    UTILS_PROC Double_List prepend_prev(T item) {
        Node *it = alloc<Node>(1, allocator);

        it->head = item;
        it->next = non_nil(node)->next;
        it->prev = node;

        non_nil(non_nil(node)->next)->prev = it;
        non_nil(node)->next = it;

        swap(node, it);

        return *this;
    }

    UTILS_PROC Node remove(int index = 0, int choose = 0) {
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

template <typename Key, typename Value,
          int (*cmp)(const void *, const void *) = [](const void *a,
                                                      const void *b) -> int {
              return static_cast<const Pair<Key, Value> *>(a)->key <=>
                     static_cast<const Pair<Key, Value> *>(b)->key;
          },
          Allocator *allocator = nullptr>
struct Table : public Array<Pair<Key, Value>, allocator> {
    using Item = Pair<Key, Value>;
    struct iterator {
        Item *current;

        UTILS_PROC_CONSTRUCT iterator(Item *ptr) : current(ptr) {}

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

        UTILS_PROC iterator &operator++(Value) {
            iterator result = *this;
            ++(*this);
            return result;
        }

        UTILS_PROC iterator &operator--() {
            --current;
            return *this;
        }

        UTILS_PROC iterator &operator--(Value) {
            iterator result = *this;
            --(*this);
            return result;
        }

        UTILS_PROC Item &operator*() { return *current; }
        UTILS_PROC Item *operator->() { return current; }
    };

    using iterator_type = iterator;
    using iterator_it_type = Item;

    UTILS_PROC iterator begin() { return iterator(&this->at(0)); }
    UTILS_PROC iterator end() { return iterator(&this->at_raw(this->length)); }

    UTILS_PROC Maybe<Item *> search(const Key &key) const {
        for (usize i = 0; i < this->length; i++) {
            Item *it = &this->at(i);

            if (it->key == key) {
                return some(it);
            }
        }

        return none<Item *>();
    }

    UTILS_PROC Table put(Item item) {
        if (auto it = search(item.key); it) {
            (*it)->value = item.value;
        }

        this->sorted_insert(item);
        return *this;
    }

    UTILS_PROC Table put(std::initializer_list<Item> items) {
        this->reserve(items.size());

        for (const auto &item : items) {
            if (auto it = search(item.key); it) {
                (*it)->value = item.value;
                continue;
            }

            this->push(item);
        }

        this->sort(cmp);
        return *this;
    }

    UTILS_PROC Maybe<Value> get(const Key &key) const {
        if (auto it = search(key); it) {
            return some((*it)->value);
        }

        return none<Value>();
    }

    UTILS_PROC Value &operator[](const Key &key) {
        if (auto it = search(key); it) {
            return (*it)->value;
        }

        this->reserve(1);

        Item *it = &this->sorted_reserve(Item(key, {}));
        it->key = key;
        it->value = {};
        return it->value;
    }
};

struct String : public Slice<char> {
    UTILS_PROC_CONSTRUCT String() : Slice() {}
    UTILS_PROC_CONSTRUCT String(Slice slice) : Slice(slice) {}

    UTILS_PROC_CONSTRUCT String(char *chars, usize length)
        : Slice(chars, length) {}
};

UTILS_PROC String operator""_s(const char *chars, usize length) {
    return String(const_cast<char *>(chars), length);
}

template <Allocator *allocator = nullptr>
struct String_Builder : public Array<char, allocator> {
    UTILS_PROC String_Builder write(char c) {
        this->push(c);
        return *this;
    }

    UTILS_PROC String_Builder write(String string) {
        for (char i = 0; i < string.length; i++) {
            write(string[i]);
        }

        return *this;
    }
};

template <usize N> struct Allocator_Static : public Allocator {
    Slice<u8> pool;
    usize used;

    UTILS_PROC_CONSTRUCT Allocator_Static()
        : pool{Slice<u8>(reinterpret_cast<u8 *>(std::malloc(N)), N)}, used{0} {}

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

                return nullptr;
            }

            UTILS_UNREACHEBLE();
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

template <typename T>
UTILS_PROC T *alloc(uint count = 1,
                    Allocator *UTILS_GLOBAL_ALLOCATOR(allocator)) {
#ifdef UTILS_NO_GLOBAL_ALLOCATOR
    UTILS_ASSERT(allocator != nullptr);
#else
    if (allocator == nullptr)
        allocator = global_allocator;
#endif
    return static_cast<T *>(allocator->run(nullptr, 0, sizeof(T) * count));
}

template <typename T>
UTILS_PROC void dealloc(T *ptr, uint count,
                        Allocator *UTILS_GLOBAL_ALLOCATOR(allocator)) {
#ifdef UTILS_NO_GLOBAL_ALLOCATOR
    UTILS_ASSERT(allocator != nullptr);
#else
    if (allocator == nullptr)
        allocator = global_allocator;
#endif
    if (ptr == nullptr || count == 0)
        return;
    allocator->run(ptr, sizeof(T) * count, 0);
}

template <typename T>
UTILS_PROC T *realloc(T *ptr, uint old_count, uint new_count,
                      Allocator *UTILS_GLOBAL_ALLOCATOR(allocator)) {
#ifdef UTILS_NO_GLOBAL_ALLOCATOR
    UTILS_ASSERT(allocator != nullptr);
#else
    if (allocator == nullptr)
        allocator = global_allocator;
#endif
    return static_cast<T *>(
        allocator->run(ptr, sizeof(T) * old_count, sizeof(T) * new_count));
}

template <typename T, Allocator *allocator = nullptr> struct Unique {
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

    UTILS_PROC void destroy() {
        if (ptr == nullptr || ptr_count == 0)
            return;

        dealloc(ptr, ptr_count, allocator);

        ptr = nullptr;
        ptr_count = 0;
    }

    UTILS_PROC_DESTRUCT ~Unique() { destroy(); }
};

template <typename T, Allocator *allocator = nullptr> struct Shared {
    struct Control_Block {
        T *ptr;
        usize ptr_count;
        uint *counter;

        UTILS_PROC_CONSTRUCT Control_Block(T *ptr, usize ptr_count,
                                           uint *counter)
            : ptr(ptr), ptr_count(ptr_count), counter(counter) {}

        UTILS_PROC_CONSTRUCT Control_Block(T *ptr, usize ptr_count)
            : ptr(ptr), ptr_count(ptr_count),
              counter(alloc<uint>(1, allocator)) {
            *counter = 1;
        }

        UTILS_PROC uint inc() { return ++(*counter); }
        UTILS_PROC uint dec() { return --(*counter); }

        UTILS_PROC void destroy() {
            if (ptr == nullptr || ptr_count == 0 || counter == nullptr ||
                *counter == 0)
                return;

            if (dec() == 0) {
                dealloc<T>(ptr, ptr_count, allocator);
                ptr = nullptr;
                ptr_count = 0;

                dealloc<uint>(counter, 1, allocator);
                counter = nullptr;

                dealloc<Control_Block>(this, 1, allocator);
            }
        }
    };

    Control_Block *control;

    UTILS_PROC_CONSTRUCT Shared(T *ptr, usize ptr_count)
        : control(alloc<Control_Block>(1, allocator)) {
        *control = Control_Block(ptr, ptr_count);
    }

    UTILS_PROC_CONSTRUCT_COPY Shared(Shared &other)
        : control(alloc<Control_Block>(1, allocator)) {
        *control = Control_Block(other.control->ptr, other.control->ptr_count,
                                 other.control->counter);
        other.control->inc();
    }

    UTILS_PROC_CONSTRUCT_MOVE Shared(Shared &&other)
        : control(Control_Block(other.control->ptr, other.control->ptr_count,
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

    UTILS_PROC T *addr() { return control->ptr; }
    UTILS_PROC T operator*() { return *addr(); }
    UTILS_PROC T *operator->() { return addr(); }

    UTILS_PROC uint counter() { return *control->counter; }

    UTILS_PROC_DESTRUCT ~Shared() { control->destroy(); }
};

UTILS_PROC_INIT void _global_allocator_destroy_init() {
    static auto _allocator = Allocator_Malloc();
    global_allocator = &_allocator;
}

UTILS_PROC_FINI void _global_allocator_destroy_fini() {
    global_allocator->destroy();
}
} // namespace utils

#ifdef UTILS_IMPLEMENT

UTILS_PROC utils::ssize utils::ptr_diff(void *a, void *b) {
    return reinterpret_cast<ssize>(a) - reinterpret_cast<ssize>(b);
}

#endif
