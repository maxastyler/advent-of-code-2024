#include "assert.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

#define INITIAL_VECTOR_SIZE 8

#define GEN_VECTOR_IMPL(vector_name, elem_type)                                \
  struct vector_name {                                                         \
    elem_type *buffer;                                                         \
    uint64_t capacity;                                                         \
    uint64_t length;                                                           \
  };                                                                           \
                                                                               \
  struct vector_name vector_name##_new() {                                     \
    uint64_t capacity = INITIAL_VECTOR_SIZE;                                   \
    elem_type *buffer = malloc(sizeof(elem_type) * capacity);                  \
    assert(buffer != NULL);                                                    \
    return (struct vector_name){                                               \
        .buffer = buffer, .capacity = capacity, .length = 0};                  \
  }                                                                            \
                                                                               \
  void vector_name##_free(struct vector_name *v) { free(v->buffer); }          \
                                                                               \
  elem_type *vector_name##_get(struct vector_name *v, uint64_t index) {        \
    if (index >= v->length) {                                                  \
      return NULL;                                                             \
    } else {                                                                   \
      return &v->buffer[index];                                                \
    }                                                                          \
  }                                                                            \
                                                                               \
  void vector_name##_double_capacity(struct vector_name *v) {                  \
    v->capacity *= 2;                                                          \
    elem_type *new_buffer =                                                    \
        realloc(v->buffer, v->capacity * sizeof(elem_type));                   \
    assert(new_buffer != NULL);                                                \
  }                                                                            \
                                                                               \
  void vector_name##_push(struct vector_name *v, elem_type elem) {             \
    if (v->length == v->capacity) {                                            \
      vector_name##_double_capacity(v);                                        \
    }                                                                          \
    v->buffer[v->length] = elem;                                               \
    v->length++;                                                               \
  }                                                                            \
                                                                               \
  void vector_name##_pop(struct vector_name *v) {                              \
    if (v->length > 0) {                                                       \
      v->length--;                                                             \
    }                                                                          \
  }                                                                            \
                                                                               \
  void vector_name##_map(struct vector_name *v, void (*mapper)(elem_type *)) { \
    for (uint64_t i = 0; i < v->length; i++) {                                 \
      mapper(&v->buffer[i]);                                                   \
    }                                                                          \
  }