#ifndef ZDS_OPTIONAL_H
#define ZDS_OPTIONAL_H

#include <cstdint>

template<typename T>
class Optional {
public:
  Optional(T value) : has_value{true}, data{value} {};
  Optional() : has_value{false} {};

  operator bool() { return this->has_value; }
  T operator*() { return this->data; }

  T operator||(T fallback) { return this->has_value ? this->data : fallback; }
private:
  bool has_value;
  T data;
};

class RNG64 {
public:
  RNG64(const char *seed, uint32_t len);
  uint64_t next();
  uint64_t bounded(uint64_t range);
  double uniform();
private:
  uint64_t states[4];
};

class RNG32 {
public:
  RNG32(const char *seed, uint32_t len);
  uint32_t next();
  uint32_t bounded(uint32_t range);
  float uniform();
private:
  uint64_t state;
};

template<typename T, const T PRIME>
constexpr uint32_t fnv_1a(const char *str, uint32_t len, uint64_t seed) {
  for (uint32_t i = 0; i < len; ++i) {
    seed ^= (uint8_t)str[i];
    seed *= PRIME;
  }

  for (uint32_t t = len; t; t >>= 8) {
    seed ^= t & 0xff;
    seed *= PRIME;
  }

  return seed;
}

constexpr uint64_t hash64(const char *str, uint32_t len, uint64_t seed) {
  return fnv_1a<uint64_t, 0x100000001b3>(str, len, seed);
}

constexpr uint32_t hash32(const char *str, uint32_t len, uint32_t seed) {
  return fnv_1a<uint32_t, 0x1000193>(str, len, seed);
}

#endif
