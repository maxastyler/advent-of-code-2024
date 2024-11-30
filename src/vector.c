#include "stdint.h"
#include "stdlib.h"

struct vector {
  // pointer to the allocated memory
  void *buffer;
  // current capacity of the buffer
  size_t capacity;
  // current valid length
  size_t length;
  size_t stride;
};

/*
  Create a vector with no elements
 */
struct vector vector_new() {
  return (struct vector){
      .buffer = NULL,
      .capacity = 0,
      .length = 0,
  };
}

/*
Allocate a vector with the given capacity,
 */
struct vector vector_with_capacity(size_t capacity) {
  void *buffer = malloc(capacity);
  return (struct vector){
      .buffer = buffer,
      .capacity = capacity,
      .length = 0,
  };
}

/*
Take the pointer to the pointer to the vector, free it, then set the pointer to
NULL
 */
void vector_free(struct vector **ptr) {
  free((*ptr)->buffer);
  *ptr = NULL;
}

/*
  Take the pointer to the pointer to the vector, run the drop fn on all
  elements, free the vector, then set the pointer to NULL
 */
void vector_free_with_drop(struct vector **ptr, void (*dropper)(void *)) {
  for (size_t i = 0; i < (*ptr)->length; i++) {
    
  }
}

int main(void) { return 0; }
