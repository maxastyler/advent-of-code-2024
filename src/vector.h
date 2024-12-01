#pragma once
#include "stdlib.h"

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

// allocate a new vector
struct vector vector_new(size_t stride, elem_drop_fun *dropper);

// free the vector, calling the dropper fn on each element
void vector_free(struct vector *v);

// free the vector without running the drop function
void vector_free_without_drop(struct vector *v);

// get a pointer to the given index
void *vector_item(struct vector const *v, size_t index);

// push the given item into the vector
void vector_push(struct vector *v, void *item);

// pop the last element of the vector. If item is not NULL, then it places the
// popped value into item
void vector_pop(struct vector *v);
