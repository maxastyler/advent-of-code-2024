#include "gen_vector.h"
#include "stdint.h"
#include "stdio.h"

GEN_VECTOR_IMPL(int_vec, uint64_t)

void print_int(uint64_t *i, void *n) {
  *(uint64_t *)n += 1;
  printf("This element is: %lu\n", *i);
}

int main(void) {
  struct int_vec v = int_vec_new();
  int_vec_push(&v, 3);
  int_vec_push(&v, 4);
  assert(v.length == 2);
  int_vec_pop(&v);
  int_vec_pop(&v);
  assert(v.length == 0);
  int_vec_pop(&v);
  assert(v.length == 0);
  uint64_t n = 0;
  int_vec_map(&v, print_int, &n);
  assert(n == 0);
  int_vec_push(&v, 3);
  int_vec_push(&v, 4);
  assert(v.length == 2);
  int_vec_map(&v, print_int, &n);
  assert(n == 2);
  struct int_vec v2;
  int_vec_clone_into(&v2, &v);
  assert(v2.length == v.length);
  for (uint64_t i = 0; i < v.length; i++) {
    assert(v2.buffer[i] == v.buffer[i]);
  }
  struct int_vec_slice vs = int_vec_get_slice(&v2, 0, 2);
  assert(vs.length == v2.length);
  for (uint64_t i = 0; i < vs.length; i++) {
    assert(vs.buffer[i] == v2.buffer[i]);
  }
  v2.buffer[0] = 3;
  for (uint64_t i = 0; i < vs.length; i++) {
    assert(vs.buffer[i] == v2.buffer[i]);
  }
  return 0;
}
