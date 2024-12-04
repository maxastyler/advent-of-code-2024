#include "../aoclib.h"
#include "assert.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

const char *mul_pat = "mul\\(([[:digit:]]{1,3}),([[:digit:]]{1,3})\\)";
const char *all_pat =
    "(mul\\(([[:digit:]]{1,3}),([[:digit:]]{1,3})\\))|(do\\(\\))|(don't\\(\\))";

uint64_t part_1(const char *input) {

  struct reg_iter r = reg_iter_new(mul_pat, input, 3);
  uint64_t total = 0;
  while (reg_iter_next_match(&r) == 0) {
    uint64_t n1 = strtol(reg_iter_string_for_match(&r, 1), NULL, 10);
    uint64_t n2 = strtol(reg_iter_string_for_match(&r, 2), NULL, 10);
    total += n1 * n2;
  }

  reg_iter_free(&r);
  return total;
}

uint64_t part_2(const char *input) {
  struct reg_iter r = reg_iter_new(all_pat, input, 4);
  bool do_mul = true;
  uint64_t total = 0;

  while (reg_iter_next_match(&r) == 0) {
    const char *full_match = reg_iter_string_for_match(&r, 0);
    if (strncmp(full_match, "do()", 4) == 0) {
      do_mul = true;
    } else if (strncmp(full_match, "don't()", 7) == 0) {
      do_mul = false;

    } else if (do_mul) {
      uint64_t n1 = strtol(reg_iter_string_for_match(&r, 2), NULL, 10);
      uint64_t n2 = strtol(reg_iter_string_for_match(&r, 3), NULL, 10);
      total += n1 * n2;
    }
  }
  reg_iter_free(&r);
  return total;
}

void run() {
  const char *input = read_file("src/day_03/input");
  printf("Part 1: %lu\n", part_1(input));
  printf("Part 2: %lu\n", part_2(input));
  free(input);
}

int main(void) {
  TIME_FUNCTION_EXECUTION(run, 100);
  return 0;
}
