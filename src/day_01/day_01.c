#include "../aoclib.h"
#include "../hashmap.h"
#include "../vector.h"
#include "assert.h"
#include "regex.h"
#include "stdbool.h"
#include "stdio.h"

/* Comparison function. Receives two generic (void) pointers to the items under
 * comparison. */
int compare_ints(const void *p, const void *q) {
  long x = *(const long *)p;
  long y = *(const long *)q;

  /* Avoid return x - y, which can cause undefined behaviour
     because of signed integer overflow. */
  if (x < y)
    return -1; // Return -1 if you want ascending, 1 if you want descending
               // order.
  else if (x > y)
    return 1; // Return 1 if you want ascending, -1 if you want descending
              // order.

  return 0;
}

bool value_key(const void *p, const void *q) {
  return *(long *)p == *(long *)q;
}

void read_file_d1(const char *fname, struct vector *col1, struct vector *col2) {

  regex_t reg;
  assert(regcomp(&reg, "([[:digit:]]+)[[:blank:]]+([[:digit:]]+)",
                 REG_EXTENDED) == 0);

  FILE *f = fopen(fname, "r");
  assert(f != NULL);

  while (true) {

    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, f) == -1) {
      free(line);
      break;
    } else {

      int n_matches = 3;
      regmatch_t matches[n_matches] = {};
      int ret = regexec(&reg, line, n_matches, matches, 0);

      if (ret != 0) {
        size_t length = regerror(ret, &reg, NULL, 0);
        char *err_buf = malloc(length);
        regerror(ret, &reg, err_buf, length);
        free(err_buf);
      } else {
        long n = strtol(line + matches[1].rm_so, NULL, 10);
        vector_push(col1, (void *)&n);
        n = strtol(line + matches[2].rm_so, NULL, 10);
        vector_push(col2, (void *)&n);
      }
      free(line);
    }
  }

  regfree(&reg);
  fclose(f);
};

long part_1(struct vector const *col1, struct vector const *col2) {
  long total = 0;
  for (size_t i = 0; i < col1->length; i++) {
    long a = *(long *)vector_item(col1, i);
    long b = *(long *)vector_item(col2, i);

    total += a > b ? a - b : b - a;
  }

  return total;
}

long part_2(struct vector const *col1, struct vector const *col2) {
  struct hashmap h = hashmap_new_default(sizeof(long), sizeof(long), value_key);
  for (size_t i; i < col2->length; i++) {
    long n = *(long *)vector_item(col2, i);
    void *value = hashmap_get(&h, &n);
    long start_count = 1;
    if (value == NULL) {
      hashmap_insert(&h, &n, &start_count);
    } else {
      *(long *)value += 1;
    }
  }
  long total = 0;
  for (size_t i; i < col1->length; i++) {
    long a = *(long *)vector_item(col1, i);
    void *b_ptr = hashmap_get(&h, &a);
    if (b_ptr != NULL) {
      total += (*(long *)b_ptr) * a;
    }
  }
  return total;
}

void run() {
  struct vector col1 = vector_new(sizeof(long), NULL);
  struct vector col2 = vector_new(sizeof(long), NULL);

  read_file_d1("src/day_01/input", &col1, &col2);

  qsort(col1.buffer, col1.length, col1.stride, compare_ints);
  qsort(col2.buffer, col2.length, col2.stride, compare_ints);

  printf("Part 1: %lu\n", part_1(&col1, &col2));
  printf("Part 2: %lu\n", part_2(&col1, &col2));
}

int main(void) {
  TIME_FUNCTION_EXECUTION(run, 100);
  return 0;
}
