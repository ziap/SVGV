#include "InverseIndex.h"

InverseIndex<0>::InverseIndex(const std::string_view *map, uint32_t count) 
  : idx_map{map},
    ht_shift{(uint32_t)__builtin_clz(count - 1) - 1},
    ht_size{(uint32_t)1 << (32 - ht_shift)},
    keys{std::make_unique<int[]>(ht_size)} {
  for (uint32_t i = 0; i < count; ++i) {
    uint32_t h = hash32(map[i].data(), map[i].size(), 0x811c9dc5) >> ht_shift;
    while (keys[h]) {
      h = (h + 1) & (ht_size - 1);
    }
    keys[h] = i + 1;
  }
}

int InverseIndex<0>::operator[](std::string_view sv) const {
  uint32_t h = hash32(sv.data(), sv.size(), 0x811c9dc5) >> ht_shift;
  while (keys[h] && idx_map[keys[h] - 1] != sv) {
    h = (h + 1) & (ht_size - 1);
  }
  return keys[h] - 1;
}
