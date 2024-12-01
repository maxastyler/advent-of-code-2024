#include "assert.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define SWAP(T, a, b)                                                          \
  do {                                                                         \
    T tmp = a;                                                                 \
    a = b;                                                                     \
    b = tmp;                                                                   \
  } while (0)

const size_t NEW_HASHMAP_SIZE = 8;
const double OCCUPANCY_THRESHOLD = 0.7;

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
                           drop_fn *value_dropper) {
  enum hashmap_slot_occupancy *occupancies =
      malloc(NEW_HASHMAP_SIZE * sizeof(enum hashmap_slot_occupancy));
  for (size_t i = 0; i < NEW_HASHMAP_SIZE; i++) {
    occupancies[i] = hashmap_empty;
  }
  char *keys = malloc(NEW_HASHMAP_SIZE * key_stride);
  char *values = malloc(NEW_HASHMAP_SIZE * value_stride);
  struct hashmap hm = {
      .occupancies = occupancies,
      .keys = keys,
      .values = values,
      .capacity = NEW_HASHMAP_SIZE,
      .n_occupations = 0,
      .length = 0,
      .hasher = hasher,
      .to_bytes = to_bytes,
      .key_eq = key_eq,
      .key_dropper = key_dropper,
      .value_dropper = value_dropper,
      .key_stride = key_stride,
      .value_stride = value_stride,
  };
  return hm;
}

int64_t hashmap_fnv1a(const char *data, size_t data_len) {
  int64_t hash = 0xcbf29ce484222325;
  const int64_t fnv_prime = 0x100000001b3;
  for (size_t i = 0; i < data_len; i++) {
    hash = (hash ^ data[i]) * fnv_prime;
  }
  return hash;
}

struct hashmap_key_bytes hashmap_default_to_bytes(void *value,
                                                  size_t key_stride) {
  struct hashmap_key_bytes hkb = {.data = value, .length = key_stride};
  return hkb;
}

// initialise a new hashmap with the default hasher and no drop functions
struct hashmap hashmap_new_default(size_t key_stride, size_t value_stride,
                                   key_eq *key_eq) {
  return hashmap_new(key_stride, value_stride, hashmap_fnv1a,
                     hashmap_default_to_bytes, key_eq, NULL, NULL);
}

void hashmap_drop_index(struct hashmap *h, size_t index) {
  if (h->key_dropper != NULL) {
    h->key_dropper(h->keys + (index * h->key_stride));
  }
  if (h->value_dropper != NULL) {
    h->value_dropper(h->values + (index * h->value_stride));
  }
}

// free hashmap's buffers, running the drop function on all keys and values
void hashmap_free(struct hashmap *h) {
  if ((h->key_dropper != NULL) || (h->value_dropper != NULL)) {
    // we need to run at least one dropper on the array elements
    // let branch prediction take care of checking the existence of these
    // functions inside this loop
    for (size_t i = 0; i < h->capacity; i++) {
      if (h->occupancies[i] == hashmap_filled) {
        hashmap_drop_index(h, i);
      }
    }
  }

  free(h->occupancies);
  free(h->keys);
  free(h->values);
}

void hashmap_insert(struct hashmap *h, void *key, void *value);

// double the capacity of the hashmap, reinserting all elements
void hashmap_double_capacity(struct hashmap *h) {
  size_t old_capacity = h->capacity;
  size_t new_capacity = h->capacity * 2;
  enum hashmap_slot_occupancy *new_occupancies =
      malloc(new_capacity * sizeof(enum hashmap_slot_occupancy));
  assert(new_occupancies != NULL);
  char *new_keys = malloc(new_capacity * h->key_stride);
  assert(new_keys != NULL);
  char *new_values = malloc(new_capacity * h->value_stride);
  assert(new_values != NULL);
  SWAP(enum hashmap_slot_occupancy *, new_occupancies, h->occupancies);
  SWAP(char *, new_keys, h->keys);
  SWAP(char *, new_values, h->values);
  h->capacity = new_capacity;
  h->n_occupations = 0;
  h->length = 0;
  for (size_t i = 0; i < old_capacity; i++) {
    if (new_occupancies[i] == hashmap_filled) {
      hashmap_insert(h, new_keys + (i * h->key_stride),
                     new_values + (i * h->value_stride));
    }
  }
  free(new_occupancies);
  free(new_keys);
  free(new_values);
}

bool hashmap_keys_equal(struct hashmap *h, void *key, size_t index) {
  return h->key_eq(key, h->keys + (index * h->key_stride));
}

// find the first index that matches or can hold the given key, using linear
// stepping
size_t hashmap_find_index(struct hashmap *h, void *key) {
  struct hashmap_key_bytes hkb = h->to_bytes(key, h->key_stride);
  size_t base_index = h->hasher(hkb.data, hkb.length) % h->capacity;
  size_t index = base_index;

  do {
    enum hashmap_slot_occupancy occ = h->occupancies[index];
    printf("index: %lu, base_index: %lu, occ: %u\n", index, base_index, occ);
    if (occ == hashmap_filled) {
      if (hashmap_keys_equal(h, key, index)) {
        return index;
      }
    } else {
      return index;
    }
    index++;
    if (index >= h->capacity) {
      index = 0;
    }

  } while (index != base_index);

  assert(index != base_index);
  return index;
}

void hashmap_check_occupancy(struct hashmap *h) {
  double occ_fraction = ((double)h->n_occupations / (double)h->capacity);
  if (occ_fraction > OCCUPANCY_THRESHOLD) {
    hashmap_double_capacity(h);
  }
}

// insert a value at the given key, replacing (and dropping) the previous
// key/value if there was one
void hashmap_insert(struct hashmap *h, void *key, void *value) {
  hashmap_check_occupancy(h);

  size_t index = hashmap_find_index(h, key);
  printf("index: %lu\n", index);
  if (h->occupancies[index] == hashmap_filled) {
    // need to drop the previous key and value
    hashmap_drop_index(h, index);
  } else if (h->occupancies[index] == hashmap_empty) {
    h->length++;
    h->n_occupations++;
  } else {
    // tombstone, just increase length
    h->length++;
  }
  h->occupancies[index] = hashmap_filled;
  memcpy(h->keys + (h->key_stride * index), key, h->key_stride);
  memcpy(h->values + (h->value_stride * index), value, h->value_stride);
}

// get the corresponding value at the given key, returning NULL if there was no
// key
void *hashmap_get(struct hashmap *h, void *key) {
  size_t index = hashmap_find_index(h, key);
  if (h->occupancies[index] == hashmap_filled) {
    return h->values + (h->value_stride * index);
  } else {
    return NULL;
  }
}

// remove the value at the given key, dropping both the key and value
// returns: true if a value was removed, false if no value was removed
bool hashmap_remove(struct hashmap *h, void *key) {
  size_t index = hashmap_find_index(h, key);
  if (h->occupancies[index] == hashmap_filled) {
    hashmap_drop_index(h, index);
    h->occupancies[index] = hashmap_tombstone;
    h->length--;
    return true;
  } else {
    return false;
  }
}
