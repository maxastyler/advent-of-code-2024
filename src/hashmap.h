#include "assert.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

enum hashmap_slot_occupancy {
  hashmap_empty,
  hashmap_filled,
  hashmap_tombstone
};
struct hashmap_key_bytes {
  const char *data;
  size_t length;
};
typedef int64_t(hash_fn)(const char *data, size_t length);
typedef struct hashmap_key_bytes(to_bytes_fn)(void *value, size_t key_stride);
typedef bool(key_eq)(const void *k1, const void *k2);
typedef void(drop_fn)(void *p);

struct hashmap {
  enum hashmap_slot_occupancy *occupancies;
  char *keys;
  char *values;
  size_t capacity;
  size_t n_occupations;
  size_t length;
  hash_fn *hasher;
  to_bytes_fn *to_bytes;
  key_eq *key_eq;
  drop_fn *key_dropper;
  drop_fn *value_dropper;
  size_t key_stride;
  size_t value_stride;
};

// initialise a new hashmap
struct hashmap hashmap_new(size_t key_stride, size_t value_stride,
                           hash_fn *hasher, to_bytes_fn *to_bytes,
                           key_eq *key_eq, drop_fn *key_dropper,
                           drop_fn *value_dropper);

int64_t hashmap_fnv1a(const char *data, size_t data_len);

// initialise a new hashmap with the default hasher and no drop functions
struct hashmap hashmap_new_default(size_t key_stride, size_t value_stride,
                                   key_eq *key_eq);

// free hashmap's buffers, running the drop function on all keys and values
void hashmap_free(struct hashmap *h);

// insert a value at the given key, replacing (and dropping) the previous
// key/value if there was one
void hashmap_insert(struct hashmap *h, void *key, void *value);

// get the corresponding value at the given key, returning NULL if there was no
// key
void *hashmap_get(struct hashmap *h, void *key);

// remove the value at the given key, dropping both the key and value
// returns: true if a value was removed, false if no value was removed
bool hashmap_remove(struct hashmap *h, void *key);
