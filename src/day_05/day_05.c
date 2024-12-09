#include "../aoclib.h"
#include "../gen_hashmap.h"
#include "../vector.h"
#include "stdint.h"

bool ikey_eq(const void *k1, const void *k2) {
  return *((uint32_t *)k1) == (*(uint32_t *)k2);
}

void value_dropper(void *v) { vector_free((struct vector *)v); }


struct hashmap parse_rules(const char *rules) {
  struct spliterator s = spliterator_new(rules, "\n");
  struct hashmap r =
      hashmap_new_default(sizeof(uint32_t *), sizeof(struct vector *), ikey_eq);
  r.value_dropper = value_dropper;
  r.key_dropper = free;
  r.to_bytes = 
  char *line = NULL;
  while ((line = spliterator_next(&s)) != NULL) {
    uint32_t *left = malloc(sizeof(uint32_t));
    uint32_t *right = malloc(sizeof(uint32_t));
    *left = strtol(line, &line, 10);
    *right = strtol(line + 1, NULL, 10);
    struct vector *v = hashmap_get(&r, left);
    if (v == NULL) {
      printf("Vector is null\n");
      v = (struct vector *)malloc(sizeof(struct vector));
      *v = vector_new(sizeof(uint32_t *), free);
      hashmap_insert(&r, left, v);
    }
    printf("Length of vector: %lu\n", v->length);
    vector_push(v, right);
  }
  spliterator_free(&s);
  return r;
}

void parse_input(const char *filename) {
  const char *input = read_file(filename);
  struct spliterator s = spliterator_new(input, "\n\n");
  const char *rules = spliterator_next(&s);
  struct hashmap r = parse_rules(rules);
  const char *books = spliterator_next(&s);
  spliterator_free(&s);
  free(input);
}

int main(void) {
  parse_input("src/day_05/input");
  return 0;
}
