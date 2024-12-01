#include "assert.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define DEFAULT_VEC_CAPACITY 1

typedef void(elem_drop_fun)(void *);

struct vector {
  // pointer to the allocated memory
  char *buffer;
  // current capacity of the buffer
  size_t capacity;
  // current valid length
  size_t length;
  size_t stride;
  elem_drop_fun *dropper;
};

// double the capacity of the vector
void vector_double_allocation(struct vector *v) {
  size_t new_capacity = v->capacity * 2;
  void *new_pointer = realloc((void *)v->buffer, new_capacity * v->stride);
  assert(new_pointer != NULL);
  v->buffer = (char *)new_pointer;
  v->capacity = new_capacity;
  assert(v->length < v->capacity);
}

/*
  Create a vector with no elements
 */
struct vector vector_new(size_t stride, elem_drop_fun *dropper) {
  assert(stride > 0);

  char *buffer = malloc(DEFAULT_VEC_CAPACITY * stride);
  assert(buffer != NULL);
  return (struct vector){
      .buffer = buffer,
      .capacity = DEFAULT_VEC_CAPACITY,
      .length = 0,
      .stride = stride,
      .dropper = dropper,
  };
}

/*
  Take the pointer to the pointer to the vector, run the drop fn on all
  elements, free the vector, then set the pointer to NULL
 */
void vector_free(struct vector *v) {
  if (v->dropper != NULL) {
    for (size_t i = 0; i < v->length; i++) {
      char *p = v->buffer + (i * v->stride);
      v->dropper((void *)p);
    }
  }
  free(v->buffer);
}

void *vector_item(struct vector const *v, size_t index) {
  if (index >= v->length) {
    return NULL;
  } else {
    return (void *)(v->buffer + (index * v->stride));
  }
}

void vector_push(struct vector *v, void *item) {
  if (v->length >= v->capacity) {
    vector_double_allocation(v);
  }
  char *p = v->buffer + (v->length * v->stride);
  memcpy((void *)p, item, v->stride);
  v->length++;
}

void vector_pop(struct vector *v) {
  if (v->length > 0) {
    char *p = v->buffer + ((v->length - 1) * v->stride);
    if (v->dropper != NULL) {
      v->dropper((void *)p);
    }
    v->length--;
  }
}
