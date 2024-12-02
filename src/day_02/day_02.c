#include "../aoclib.h"
#include "assert.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

enum direction { NONE, INVALID, DOWN, UP };

enum direction check_direction(uint64_t current, uint64_t prev) {
  if (current == prev) {
    return INVALID;
  } else {
    uint64_t diff = current > prev ? current - prev : prev - current;
    if (current > prev) {
      return (current - prev) > 3 ? INVALID : UP;
    } else {
      return (prev - current) > 3 ? INVALID : DOWN;
    }
  }
}

bool line_is_good(const char *line, uint64_t skip_index) {
  bool ret_val = true;
  struct spliterator splits = spliterator_new(line, ' ');
  const char *n = NULL;
  enum direction dir = NONE;
  uint64_t current;

  uint64_t i = 1;

  // we should always have a first number
  assert((n = spliterator_next(&splits)) != NULL);
  if (skip_index == 0) {
    assert((n = spliterator_next(&splits)) != NULL);
    i = 2;
  }
  uint64_t prev = strtol(n, NULL, 10);

  while ((n = spliterator_next(&splits))) {
    if (i == skip_index) {
      i++;
      continue;
    }
    current = strtol(n, NULL, 10);
    enum direction new_dir = check_direction(current, prev);
    if (new_dir == INVALID) {
      ret_val = false;
      goto free_splits;
    } else if (dir == NONE) {
      dir = new_dir;
    } else if (dir != new_dir) {
      ret_val = false;
      goto free_splits;
    }
    prev = current;
    i++;
  }
free_splits:
  spliterator_free(&splits);
  return ret_val;
}

bool line_is_good_p2(const char *line) {
  for (size_t i = 0; i < 1000; i++) {
    if (line_is_good(line, i)) {
      return true;
    }
  }
  return false;
}

uint64_t part_1(const char *input) {
  struct spliterator lines = spliterator_new(input, '\n');
  const char *line = NULL;
  char *n = NULL;
  uint64_t total = 0;
  while ((line = spliterator_next(&lines))) {
    if (line_is_good(line, -1)) {
      total++;
    }
  }
  spliterator_free(&lines);
  return total;
}

uint64_t part_2(const char *input) {
  struct spliterator lines = spliterator_new(input, '\n');
  const char *line = NULL;
  char *n = NULL;
  uint64_t total = 0;
  while ((line = spliterator_next(&lines))) {
    if (line_is_good_p2(line)) {
      total++;
    }
  }
  spliterator_free(&lines);
  return total;
}

int main(void) {
  const char *input = read_file("src/day_02/input");
  printf("Part 1: %lu\n", part_1(input));
  printf("Part 2: %lu\n", part_2(input));
  return 0;
}
