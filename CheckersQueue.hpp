#pragma once
#include <cstddef>

#define CAPACITY 48

template <typename T>
class CheckersQueue {
private:
    T data[CAPACITY];
    size_t frontIndex;
    size_t backIndex;
    size_t count;

public:
    CheckersQueue() : frontIndex(0), backIndex(0), count(0) {}

    void push(const T& value) {
        if (count < CAPACITY) {
            data[backIndex] = value;
            backIndex = (backIndex + 1) % CAPACITY;
            ++count;
        }
    }

    void pop() {
        if (!empty()) {
            frontIndex = (frontIndex + 1) % CAPACITY;
            --count;
        }
    }

    T& front() { return data[frontIndex]; }
    const T& front() const { return data[frontIndex]; }

    bool empty() const { return count == 0; }
    size_t size() const { return count; }

    void clear() {
        frontIndex = 0;
        backIndex = 0;
        count = 0;
    }
};