#include "stdint.h"

int64_t fnv_hash(int64_t hash, const char *bytes, uint64_t bytes_len) {
  const int64_t fnv_prime = 0x100000001b3;
  for (uint64_t i = 0; i < bytes_len; i++) {
    hash = (hash ^ bytes[i]) * fnv_prime;
  }
  return hash;
}
