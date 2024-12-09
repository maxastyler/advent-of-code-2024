#include "gen_hashmap.h"
#include "stdint.h"

GEN_HASHMAP_DEFAULT_IMPL(int_hashmap, uint64_t, uint64_t);

void map_int(uint64_t *k, uint64_t *v, void *state) { *(uint64_t *)state += 1; }

int main(void) {
  struct int_hashmap x = int_hashmap_new();
  int_hashmap_free(&x);
  assert(int_hashmap_get(&x, 2) == NULL);
  int_hashmap_insert(&x, 2, 3);
  assert(int_hashmap_get(&x, 2) != NULL);
  assert(*int_hashmap_get(&x, 2) == 3);
  assert(int_hashmap_delete(&x, 2));
  assert(!int_hashmap_delete(&x, 2));
  assert(x.length == 0);
  assert(int_hashmap_get(&x, 2) == NULL);
  for (int i = 0; i < 100; i++) {
    int_hashmap_insert(&x, i, i);
    assert(x.length == i + 1);
  }
  assert(int_hashmap_delete(&x, 0));
  assert(x.length == 99);
  uint64_t n = 0;
  int_hashmap_map(&x, map_int, &n);
  assert(n == 99);
  return 0;
}
