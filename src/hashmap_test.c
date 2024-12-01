#include "assert.h"
#include "hashmap.h"

bool int_eq(const void *k1, const void *k2) {
  return *(size_t *)k1 == *(size_t *)k2;
}

int test_1() {
  struct hashmap h =
      hashmap_new_default(sizeof(size_t), sizeof(size_t), int_eq);
  size_t x = 0;
  assert(hashmap_get(&h, &x) == NULL);
  hashmap_insert(&h, &x, &x);
  assert(h.length == 1);
  assert(h.n_occupations == 1);
  assert(*(size_t *)hashmap_get(&h, &x) == x);
  x = 3;
  hashmap_insert(&h, &x, &x);
  assert(h.length == 2);
  assert(h.n_occupations == 2);
  size_t y = 3;
  hashmap_insert(&h, &y, &y);
  assert(h.length == 2);
  assert(h.n_occupations == 2);
  assert(*(size_t *)hashmap_get(&h, &y) == y);
  assert(hashmap_remove(&h, &y));
  assert(!hashmap_remove(&h, &y));
  assert(h.length == 1);
  assert(h.n_occupations == 2);
  assert(hashmap_get(&h, &y) == NULL);
  for (size_t i = 0; i < 100; i++) {

    hashmap_insert(&h, &i, &i);
    hashmap_insert(&h, &i, &i);
  }
  return 0;
}

int test_2() {
  struct hashmap h =
      hashmap_new_default(sizeof(size_t), sizeof(size_t), int_eq);

  for (size_t i = 0; i < 200; i++) {
    hashmap_insert(&h, &i, &i);
  }
  assert(h.length == 200);
  return 0;
}

int main(void) {
  /* if (test_1() != 0) { */
  /*   return 1; */
  /* } */
  if (test_2() != 0) {
    return 1;
  }
  return 0;
}
