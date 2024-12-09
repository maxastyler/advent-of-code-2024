#include "../aoclib.h"
#include "../gen_hashmap.h"
#include "../gen_vector.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

GEN_HASHMAP_IMPL(rules_set, uint32_t, bool);

GEN_HASHMAP_IMPL(rules_map, uint32_t, struct rules_set);

GEN_VECTOR_IMPL(pages, uint32_t);
GEN_VECTOR_IMPL(books, struct pages);

bool edge_exists(struct rules_map *r, uint32_t from, uint32_t to) {

  struct rules_set *a = rules_map_get(r, from);

  if (a == NULL) {
    return false;
  } else {
    bool *b = rules_set_get(a, to);
    return b != NULL;
  }
}

void parse_rules(const char *rules_string, struct rules_map *r) {
  struct spliterator s = spliterator_new(rules_string, "\n");
  char *line;
  while ((line = spliterator_next(&s)) != NULL) {
    uint32_t left = strtol(line, &line, 10);
    uint32_t right = strtol(line + 1, NULL, 10);
    struct rules_set *v = rules_map_get(r, left);
    if (v == NULL) {
      rules_map_insert(r, left, rules_set_new());
      v = rules_map_get(r, left);
      assert(v != NULL);
    }
    rules_set_insert(v, right, true);
  }
  spliterator_free(&s);
}

void parse_book(const char *book, struct pages *p) {
  struct spliterator page_iter = spliterator_new(book, ",");
  char *n;
  while ((n = spliterator_next(&page_iter)) != NULL) {
    pages_push(p, strtol(n, NULL, 10));
  }
  spliterator_free(&page_iter);
}

bool book_is_ordered(struct rules_map *rules, struct pages *pages) {
  for (uint64_t i = 0; i < pages->length; i++) {
    for (uint64_t j = i + 1; j < pages->length; j++) {
      if (edge_exists(rules, *pages_get(pages, j), *pages_get(pages, i))) {
        return false;
      }
    }
  }
  return true;
}

void read_input(const char *filename, struct rules_map *r, struct books *b) {
  const char *input = read_file(filename);
  struct spliterator s = spliterator_new(input, "\n\n");

  const char *rules = spliterator_next(&s);
  parse_rules(rules, r);

  const char *books_string = spliterator_next(&s);
  struct spliterator book_iter = spliterator_new(books_string, "\n");

  const char *book_string;
  struct pages pg;

  while ((book_string = spliterator_next(&book_iter)) != NULL) {
    pg = pages_new();
    parse_book(book_string, &pg);
    books_push(b, pg);
  }
  spliterator_free(&book_iter);
  spliterator_free(&s);
  free(input);
}

uint64_t part_1(struct rules_map *rules, struct books *books) {
  uint64_t total = 0;
  struct pages *pages;
  for (uint64_t i = 0; i < books->length; i++) {
    pages = books_get(books, i);
    if (book_is_ordered(rules, pages)) {
      total += *pages_get(pages, pages->length / 2);
    }
  }
  return total;
}

static struct rules_map rules_static;

int sort_fun(const void *a, const void *b) {
  if (edge_exists(&rules_static, *(uint32_t *)a, *(uint32_t *)b)) {
    return -1;
  } else if (edge_exists(&rules_static, *(uint32_t *)b, *(uint32_t *)a)) {
    return 1;
  } else {
    return 0;
  }
}

uint64_t part_2(struct books *books) {
  uint64_t total = 0;
  for (uint64_t i = 0; i < books->length; i++) {
    struct pages p = books->buffer[i];
    if (!book_is_ordered(&rules_static, &p)) {
      qsort(p.buffer, p.length, sizeof(uint32_t), sort_fun);
      total += *pages_get(&p, p.length / 2);
    }
  }
  return total;
}

int main(void) {
  struct rules_map rules = rules_map_new();
  struct books books = books_new();
  read_input("src/day_05/input", &rules, &books);
  rules_static = rules;
  printf("Part 1: %lu\n", part_1(&rules, &books));
  printf("Part 2: %lu\n", part_2(&books));
  return 0;
}
