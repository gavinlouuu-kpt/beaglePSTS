#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <cstddef>
#include <cstdint>

class CircularBuffer {
public:
    CircularBuffer(size_t size);
    ~CircularBuffer();

    void push(uint8_t data);
    bool pop(uint8_t &data);
    bool isEmpty() const;
    bool isFull() const;
    void clear();

private:
    uint8_t* buf_;
    size_t max_size_;
    size_t head_;
    size_t tail_;
    bool full_;
};

#endif // CIRCULARBUFFER_H
