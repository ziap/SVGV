#include "utils.h"

#include <cstring>
#include <bit>

static void mul_eq_256(uint32_t lhs[8], uint32_t rhs[8]) {
  uint32_t res[8] = {0};
  for (int i = 0; i < 8; ++i) {
    uint64_t c = 0;
    for (int j = 0; j + i < 8; ++j) {
      c += (uint64_t)res[i + j] + (uint64_t)lhs[i] * (uint64_t)rhs[j];
      res[i + j] = c & 0xffffffff;
      c >>= 32;
    }
  }
  memcpy(lhs, res, sizeof(res));
}

RNG64::RNG64(const char *seed, uint32_t len) {
  uint32_t fnv_prime[8] = {0x163, 0, 0, 0, 0, 0x100, 0, 0};
  uint32_t s[8] = {
    0xcaee0535, 0x1023b4c8, 0x47b6bbb3, 0xc8b15368,
    0xc4e576cc, 0x2d98c384, 0xaac55036, 0xdd268dbc
  };

  for (uint32_t t = len; t; t >>= 8) {
    s[0] ^= t & 0xff;
    mul_eq_256(s, fnv_prime);
  }

  for (uint32_t i = 0; i < len; ++i) {
    s[0] ^= (uint8_t)seed[i];
    mul_eq_256(s, fnv_prime);
  }

  this->states[0] = (uint64_t)s[0] | (((uint64_t)s[1]) << 32);
  this->states[1] = (uint64_t)s[2] | (((uint64_t)s[3]) << 32);
  this->states[2] = (uint64_t)s[4] | (((uint64_t)s[5]) << 32);
  this->states[3] = (uint64_t)s[6] | (((uint64_t)s[7]) << 32);

  this->next();
  this->next();
  this->next();
  this->next();
}

uint64_t RNG64::next() {
  uint64_t s = this->states[0] + this->states[3];
  uint64_t r = ((s << 23) | (s >> 41)) + this->states[0];

  uint64_t t = this->states[1] << 17;

  this->states[2] ^= this->states[0];
  this->states[3] ^= this->states[1];
  this->states[1] ^= this->states[2];
  this->states[0] ^= this->states[3];

  this->states[2] ^= t;

  this->states[3] = (this->states[3] << 45) | (this->states[3] >> 19);

  return r;
}

double RNG64::uniform() {
  union {
    uint64_t u;
    double f;
  } buf;

  buf.f = 1;
  buf.u |= this->next() >> 12;
  return buf.f - 1;
}

uint64_t RNG64::bounded(uint64_t range) {
  uint64_t mask = (uint64_t)-1 >> std::countl_zero(range - 1);
  uint64_t result;
  do {
    result = this->next() & mask;
  } while (result >= range);
  return result;
}

RNG32::RNG32(const char *seed, uint32_t len) {
  this->state = hash64(seed, len, 0xcbf29ce484222325);
  this->next();
}

uint32_t RNG32::next() {
  uint64_t s = this->state;
  this->state = this->state * 0x5851f42d4c957f2d + 0x14057b7ef767814f;

  uint32_t xorshifted = ((s >> 18u) ^ s) >> 27u;
  uint32_t rot = s >> 59u;
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

float RNG32::uniform() {
  union {
    uint32_t u;
    float f;
  } buf;

  buf.f = 1;
  buf.u |= this->next() >> 9;
  return buf.f - 1;
}

uint32_t RNG32::bounded(uint32_t range) {
  uint32_t t = (-range) % range;
  uint64_t m;

  do {
    m = (uint64_t)this->next() * (uint64_t)range;
  } while ((uint32_t)m < t);

  return m >> 32;
}
