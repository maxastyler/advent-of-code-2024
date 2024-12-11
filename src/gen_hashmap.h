#pragma once

#include "assert.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"

enum hashmap_occupancy { hashmap_empty, hashmap_filled, hashmap_tombstone };

#define FNV_START 0xcbf29ce484222325

#define INITIAL_HASHMAP_SIZE_EXP 3

#define OCCUPANCY_THRESHOLD 0.7

int64_t fnv_hash(int64_t hash, const char *bytes, uint64_t bytes_len);

#define GEN_HASHMAP_IMPL(hashmap_name, key_type, value_type, equality_expr,    \
                         hash_expr)                                            \
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
  bool hashmap_name##_keys_equal(const key_type *a, const key_type *b) {       \
    return equality_expr;                                                      \
  }                                                                            \
                                                                               \
  int64_t hashmap_name##_hash_key(const key_type *key) { return hash_expr; }   \
                                                                               \
  uint64_t hashmap_name##_find_index(struct hashmap_name *h, key_type key) {   \
    uint64_t base_index = hashmap_name##_hash_key(&key) & (h->capacity - 1);   \
    uint64_t index = base_index;                                               \
    do {                                                                       \
      if (h->entries[index].occupancy == hashmap_filled) {                     \
        if (hashmap_name##_keys_equal(&key, &h->entries[index].key)) {         \
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
    struct hashmap_name##_entry *old_entries = h->entries;                     \
    struct hashmap_name##_entry *new_entries =                                 \
        hashmap_name##_generate_entries(h->capacity);                          \
    assert(new_entries != NULL);                                               \
    h->entries = new_entries;                                                  \
    h->length = 0;                                                             \
    h->n_occupations = 0;                                                      \
    for (uint64_t i = 0; i < old_capacity; i++) {                              \
      if (old_entries[i].occupancy == hashmap_filled) {                        \
        key_type key = old_entries[i].key;                                     \
        value_type value = old_entries[i].value;                               \
        hashmap_name##_insert_unchecked(h, key, value);                        \
      }                                                                        \
    }                                                                          \
    free(old_entries);                                                         \
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
  }                                                                            \
                                                                               \
  void hashmap_name##_map(struct hashmap_name *h,                              \
                          void (*mapper)(key_type *, value_type *, void *),    \
                          void *map_state) {                                   \
    for (uint64_t i = 0; i < h->capacity; i++) {                               \
      if (h->entries[i].occupancy == hashmap_filled) {                         \
        mapper(&h->entries[i].key, &h->entries[i].value, map_state);           \
      }                                                                        \
    }                                                                          \
  }

#define GEN_HASHMAP_DEFAULT_IMPL(hashmap_name, key_type, value_type)           \
  GEN_HASHMAP_IMPL(hashmap_name, key_type, value_type, *a == *b,               \
                   fnv_hash(FNV_START, (const char *)key, sizeof(key_type)))
