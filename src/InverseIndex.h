#ifndef ZDS_INVERSE_INDEX_H
#define ZDS_INVERSE_INDEX_H

#include <cstdint>
#include <string_view>
#include <memory>

#include "utils.h"

constexpr uint32_t compute_shift(uint32_t x) {
  uint32_t res = 1;
  uint32_t shift = 31;
  while (res < x) {
    res <<= 1;
    --shift;
  }
  return shift;
}

template<uint32_t N>
struct InverseIndex {
public:
  constexpr InverseIndex(const std::string_view (*map)[N])
      : idx_map{*map}, keys{{0}} {
    for (uint32_t i = 0; i < N; ++i) {
      uint32_t h = hash32((*map)[i].data(), (*map)[i].size(), 0x811c9dc5) >> ht_shift;
      while (keys[h]) {
        h = (h + 1) & (ht_size - 1);
      }
      keys[h] = i + 1;
    }
  }

  constexpr int operator[](std::string_view sv) const {
    uint32_t h = hash32(sv.data(), sv.size(), 0x811c9dc5) >> ht_shift;
    while (keys[h] && idx_map[keys[h] - 1] != sv) {
      h = (h + 1) & (ht_size - 1);
    }
    return keys[h] - 1;
  }
private:
  static constexpr uint32_t ht_shift = compute_shift(N);
  static constexpr uint32_t ht_size = 1 << (32 - ht_shift);

  const std::string_view *idx_map;
  int keys[ht_size];
};

template<>
struct InverseIndex<0> {
public:
  InverseIndex(const std::string_view *map, uint32_t count);
  int operator[](std::string_view sv) const;
private:
  const std::string_view *idx_map;
  const uint32_t ht_shift;
  const uint32_t ht_size;
  const std::unique_ptr<int[]> keys;
};

#endif
