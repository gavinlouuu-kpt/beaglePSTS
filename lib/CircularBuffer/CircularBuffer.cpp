#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(size_t size) 
    : buf_(new uint8_t[size]), max_size_(size), head_(0), tail_(0), full_(false) {}

CircularBuffer::~CircularBuffer() {
    delete[] buf_;
}

void CircularBuffer::push(uint8_t data) {
    buf_[head_] = data;
    if (full_) {
        tail_ = (tail_ + 1) % max_size_;
    }
    head_ = (head_ + 1) % max_size_;
    full_ = head_ == tail_;
}

bool CircularBuffer::pop(uint8_t &data) {
    if (isEmpty()) {
        return false;
    }
    data = buf_[tail_];
    tail_ = (tail_ + 1) % max_size_;
    full_ = false;
    return true;
}

bool CircularBuffer::isEmpty() const {
    return (!full_ && (head_ == tail_));
}

bool CircularBuffer::isFull() const {
    return full_;
}

void CircularBuffer::clear() {
    head_ = tail_;
    full_ = false;
}
