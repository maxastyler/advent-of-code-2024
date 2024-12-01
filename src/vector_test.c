#include "assert.h"
#include "stdint.h"
#include "vector.h"

static int32_t n_drop = 0;

void empty_drop(void *elem) { n_drop++; }

void test_vector_create_free() {
  struct vector v = vector_new(sizeof(int32_t), empty_drop);
  int32_t x = 3;
  vector_push(&v, &x);
  assert(v.length == 1);
  vector_push(&v, &x);
  assert(v.length == 2);
  vector_pop(&v);
  assert(n_drop == 1);
  assert(v.length == 1);
  vector_free(&v);
  assert(n_drop == 2);
}

int main(void) {
  test_vector_create_free();
  return 0;
}
