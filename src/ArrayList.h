#ifndef ZDS_ARRAY_LIST_H
#define ZDS_ARRAY_LIST_H

#include <bit>
#include <iterator>
#include <memory>

#include <cstring>

#include "utils.h"

// A contiguous growable array type that support efficiently inserting and
// concatenating to the end
template<typename T>
class ArrayList {
public:
  // Adds an element to the end of the list
  void push(T value) {
    if (this->size == this->capacity) {
      if (this->capacity != 0) {
        this->capacity <<= 1;
      } else {
        this->capacity = 1;
      }
      std::unique_ptr<T[]> next_data = std::make_unique<T[]>(this->capacity);
      memcpy(next_data.get(), this->data.get(), this->size * sizeof(T));
      this->data.swap(next_data);
    }
    this->data[this->size++] = value;
  }

  // Removes an element from the end of the list, return the removed value
  Optional<T> pop() { 
    if (this->size > 0) return Optional<T>::some(this->data[--this->size]);
    return Optional<T>::none();
  }

  // Reserves capacity for at least `additional` more elements to be inserted
  void reserve(uint32_t additional) {
    if (additional > this->capacity) {
      this->capacity = (uint32_t)1 << (32 - std::countl_zero(additional - 1));
      std::unique_ptr<T[]> next_data = std::make_unique<T[]>(this->capacity);
      memcpy(next_data.get(), this->data.get(), this->size * sizeof(T));
      this->data.swap(next_data);
    }
  }

  // Resizes the list so that `.len()` is equal to `new_size`
  void resize(uint32_t new_size) {
    reserve(new_size);
    this->size = new_size;
  }

  // Appends all `size` elements from `data` to the list
  void extend(const T *data, uint32_t size) {
    uint32_t next_size = this->size + size;
    this->reserve(next_size);
    memcpy(this->data.get() + this->size, data, size * sizeof(T));
    this->size = next_size;
  }

  // Appends all elements from another `ArrayList` or static array
  template<typename T1>
  void append(const T1 &x) {
    const T *it = std::cbegin(x);
    return this->extend(it, std::cend(x) - it);
  }

  // Returns a new list with elements copied from the original list
  ArrayList clone() const {
    ArrayList result;
    result.data = std::make_unique<T[]>(this->capacity);
    memcpy(result.data.get(), this->data.get(), this->size * sizeof(T));
    result.size = this->size;
    result.capacity = this->capacity;
    return result;
  }

  // Returns the number of elements in the list
  uint32_t len() const { return this->size; }

  T &operator[](uint32_t idx) { return this->data[idx]; }
  const T &operator[](uint32_t idx) const { return this->data[idx]; }

  T *begin() { return this->data.get(); }
  T *end() { return this->data.get() + this->size; }
  const T *begin() const { return this->data.get(); }
  const T *end() const { return this->data.get() + this->size; }

private:
  static_assert(std::is_trivially_copyable<T>::value,
                "zds::ArrayList only supports trivially copiable items");

  std::unique_ptr<T[]> data = nullptr;
  uint32_t size = 0;
  uint32_t capacity = 0;
};

#endif
