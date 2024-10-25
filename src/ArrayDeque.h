#ifndef ZDS_ARRAY_DEQUE_H
#define ZDS_ARRAY_DEQUE_H

#include <memory>

#include <cstdint>
#include <cstring>

#include "utils.h"

// A double-ended queue (deque) implemented with a growable ring buffer.
template<typename T>
class ArrayDeque {
public:
  // Adds an element to the end of the deque
  void push_back(T value) {
    this->grow();
    this->data[this->tail] = value;
    this->tail = (this->tail + 1) & (this->capacity - 1);
    ++this->size;
  }

  // Adds an element to the start of the deque
  void push_front(T value) {
    this->grow();
    this->head = (this->head - 1) & (this->capacity - 1);
    this->data[this->head] = value;
    ++this->size;
  }

  // Removes an element from the end of the deque, return the removed value
  Optional<T> pop_back() {
    if (this->size > 0) {
      this->tail = (this->tail - 1) & (this->capacity - 1);
      --this->size;
      return this->data[this->tail];
    }
    return {};
  }

  // Removes an element from the start of the deque, return the removed value
  Optional<T> pop_front() {
    if (this->size > 0) {
      T item = this->data[this->head];
      this->head = (this->head + 1) & (this->capacity - 1);
      --this->size;
      return item;
    }
    return {};
  }

  // Gets and returns an element from the end of the deque without removing it
  Optional<T> peek_back() {
    if (this->size > 0) {
      return this->data[(this->tail - 1) & (this->capacity - 1)];
    }
    return {};
  }

  // Gets and returns an element from the start of the deque without removing it
  Optional<T> peek_front() {
    if (this->size > 0) {
      return this->data[this->head];
    }
    return {};
  }

  T &operator[](uint32_t idx) {
    return this->data[(this->head + idx) & (this->capacity - 1)];
  }

  const T &operator[](uint32_t idx) const {
    return this->data[(this->head + idx) & (this->capacity - 1)];
  }

  // Returns the number of elements in the deque
  uint32_t len() { return this->size; }
private:
  static_assert(std::is_trivially_copyable<T>::value,
                "zds::ArrayDeque only supports trivially copiable items");

  void grow() {
    if (this->size == this->capacity) {
      if (this->capacity != 0) {
        this->capacity <<= 1;
      } else {
        this->capacity = 2;
      }
      std::unique_ptr<T[]> next_data = std::make_unique<T[]>(this->capacity);
      size_t copy_size = this->size * sizeof(T);
      memcpy(next_data.get(), this->data.get(), copy_size);
      memcpy(next_data.get() + this->size, this->data.get(), copy_size);
      this->data.swap(next_data);
      this->tail += this->size;
    }
  }

  std::unique_ptr<T[]> data = nullptr;
  uint32_t tail = 0;
  uint32_t head = 0;
  uint32_t size = 0;
  uint32_t capacity = 0;
};

#endif
