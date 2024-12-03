#include "../aoclib.h"
#include "assert.h"
#include "regex.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

uint64_t part_1(const char *input) {
  const char *pat = "mul\\(([[:digit:]]{1,3}),([[:digit:]]{1,3})\\)";
  struct reg_iter r = reg_iter_new(pat, input, 3);
  uint64_t total = 0;
  while (reg_iter_next_match(&r) == 0) {
    uint64_t n1 = strtol(reg_iter_string_for_match(&r, 1), NULL, 10);
    uint64_t n2 = strtol(reg_iter_string_for_match(&r, 2), NULL, 10);
    total += n1 * n2;
  }
  return total;
}

int main(void) {
  const char *input = read_file("src/day_03/input");
  printf("Part 1: %lu\n", part_1(input));
  return 0;
}
