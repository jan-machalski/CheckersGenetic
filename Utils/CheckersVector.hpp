#pragma once
#include <cstddef>

#define CAPACITY 48

template <typename T>
class CheckersVector {
private:
    T data[CAPACITY];
    size_t length;

public:
    CheckersVector() : length(0) {}

    void push_back(const T& value) {
        if (length < CAPACITY) {
            data[length++] = value;
        }
    }

    T& at(size_t index) { return data[index]; }
    const T& at(size_t index) const { return data[index]; }

    size_t size() const { return length; }

    T& operator[](size_t index) { return data[index]; }
    const T& operator[](size_t index) const { return data[index]; }

    T* begin() { return data; }
    const T* begin() const { return data; }
    T* end() { return data + length; }
    const T* end() const { return data + length; }

    void clear() { length = 0; }
	bool empty() const { return length == 0; }

	int find(const T& value) const {
		for (size_t i = 0; i < length; ++i) {
			if (data[i] == value) {
				return i;
			}
		}
		return -1;
	}
};