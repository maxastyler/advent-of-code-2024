#include "../aoclib.h"

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"

uint64_t part_1(const char *input) {
  struct spliterator lines = spliterator_new(input, "\n");
  return 0;
}

int main(void) {
  const char *input = read_file("src/day_07/test");

  printf("Part 1: %lu\n", part_1(input));
  return 0;
}
