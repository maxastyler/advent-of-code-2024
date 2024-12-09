#include "assert.h"
#include "stdbool.h"
#include "stdlib.h"

enum hashmap_occupancy { hashmap_empty, hashmap_filled, hashmap_tombstone };

#define INITIAL_HASHMAP_SIZE_EXP 3

#define OCCUPANCY_THRESHOLD 0.7

#define GEN_HASHMAP_IMPL(hashmap_name, key_type, value_type)                   \
  struct hashmap_name##_entry {                                                \
    enum hashmap_occupancy occupancy;                                          \
    key_type key;                                                              \
    value_type value;                                                          \
  };                                                                           \
  struct hashmap_name {                                                        \
    struct hashmap_name##_entry *entries;                                      \
    uint64_t capacity;                                                         \
    uint8_t capacity_index;                                                    \
    uint64_t n_occupations;                                                    \
    uint64_t length;                                                           \
  };                                                                           \
  struct hashmap_name##_entry *hashmap_name##_generate_entries(                \
      uint64_t n_entries) {                                                    \
    struct hashmap_name##_entry *entries =                                     \
        malloc(n_entries * sizeof(struct hashmap_name##_entry));               \
    if (entries == NULL) {                                                     \
      return entries;                                                          \
    }                                                                          \
    for (uint64_t i = 0; i < n_entries; i++) {                                 \
      entries[i].occupancy = hashmap_empty;                                    \
    }                                                                          \
    return entries;                                                            \
  }                                                                            \
                                                                               \
  struct hashmap_name hashmap_name##_new() {                                   \
    uint8_t capacity_index = INITIAL_HASHMAP_SIZE_EXP;                         \
    uint64_t capacity = (1 << capacity_index);                                 \
    struct hashmap_name##_entry *entries =                                     \
        hashmap_name##_generate_entries(capacity);                             \
                                                                               \
    return (struct hashmap_name){.entries = entries,                           \
                                 .capacity = capacity,                         \
                                 .capacity_index = capacity_index,             \
                                 .n_occupations = 0,                           \
                                 .length = 0};                                 \
  }                                                                            \
                                                                               \
  void hashmap_name##_free(struct hashmap_name *h) { free(h->entries); }       \
                                                                               \
  int64_t hashmap_name##_hash_key(key_type key) {                              \
    const char *key_ptr = (const char *)&key;                                  \
    int64_t hash = 0xcbf29ce484222325;                                         \
    const int64_t fnv_prime = 0x100000001b3;                                   \
    for (size_t i = 0; i < sizeof(key_type); i++) {                            \
      hash = (hash ^ key_ptr[i]) * fnv_prime;                                  \
    }                                                                          \
    return hash;                                                               \
  }                                                                            \
                                                                               \
  uint64_t hashmap_name##_find_index(struct hashmap_name *h, key_type key) {   \
    uint64_t base_index = hashmap_name##_hash_key(key) & (h->capacity - 1);    \
    uint64_t index = base_index;                                               \
    do {                                                                       \
      if (h->entries[index].occupancy == hashmap_filled) {                     \
        if (key == h->entries[index].key) {                                    \
          return index;                                                        \
        }                                                                      \
      } else {                                                                 \
        return index;                                                          \
      }                                                                        \
      index++;                                                                 \
      if (index >= h->capacity) {                                              \
        index = 0;                                                             \
      }                                                                        \
    } while (index != base_index);                                             \
    assert(index != base_index);                                               \
    return index;                                                              \
  }                                                                            \
                                                                               \
  void hashmap_name##_insert_unchecked(struct hashmap_name *h, key_type key,   \
                                       value_type value) {                     \
    uint64_t index = hashmap_name##_find_index(h, key);                        \
    if (h->entries[index].occupancy == hashmap_empty) {                        \
      h->n_occupations++;                                                      \
    }                                                                          \
    h->length++;                                                               \
    h->entries[index].occupancy = hashmap_filled;                              \
    h->entries[index].key = key;                                               \
    h->entries[index].value = value;                                           \
  }                                                                            \
                                                                               \
  void hashmap_name##_double_capacity(struct hashmap_name *h) {                \
    uint64_t old_capacity = h->capacity;                                       \
    h->capacity_index++;                                                       \
    h->capacity = 1 << h->capacity_index;                                      \
    struct hashmap_name##_entry *new_entries =                                 \
        hashmap_name##_generate_entries(h->capacity);                          \
    assert(new_entries != NULL);                                               \
    for (uint64_t i = 0; i < old_capacity; i++) {                              \
      if (h->entries[i].occupancy == hashmap_filled) {                         \
        key_type key = h->entries[i].key;                                      \
        value_type value = h->entries[i].value;                                \
        hashmap_name##_insert_unchecked(h, key, value);                        \
      }                                                                        \
    }                                                                          \
    free(h->entries);                                                          \
    h->entries = new_entries;                                                  \
  }                                                                            \
                                                                               \
  void hashmap_name##_insert(struct hashmap_name *h, key_type key,             \
                             value_type value) {                               \
    double occ_fraction = ((double)h->n_occupations / (double)h->capacity);    \
    if (occ_fraction > OCCUPANCY_THRESHOLD) {                                  \
      hashmap_name##_double_capacity(h);                                       \
    }                                                                          \
    hashmap_name##_insert_unchecked(h, key, value);                            \
  }                                                                            \
  value_type *hashmap_name##_get(struct hashmap_name *h, key_type key) {       \
    uint64_t index = hashmap_name##_find_index(h, key);                        \
    if (h->entries[index].occupancy == hashmap_filled) {                       \
      return &h->entries[index].value;                                         \
    } else {                                                                   \
      return NULL;                                                             \
    }                                                                          \
  }                                                                            \
  bool hashmap_name##_delete(struct hashmap_name *h, key_type key) {           \
    uint64_t index = hashmap_name##_find_index(h, key);                        \
    if (h->entries[index].occupancy == hashmap_filled) {                       \
      h->entries[index].occupancy = hashmap_tombstone;                         \
      h->length--;                                                             \
      return true;                                                             \
    } else {                                                                   \
      return false;                                                            \
    }                                                                          \
  }
