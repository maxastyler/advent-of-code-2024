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
  return 0;
}
