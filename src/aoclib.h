#pragma once

#include "regex.h"
#include "stdbool.h"
#include "stdint.h"

// read the complete file to a string
const char *read_file(const char *fname);

struct spliterator {
  const char delim;
  char *position;
  char *buffer;
  bool finished;
};

struct spliterator spliterator_new(const char *str, const char delim);

const char *spliterator_next(struct spliterator *s);

void spliterator_free(struct spliterator *s);

// struct that iterates over multiple matches in a string
struct reg_iter {
  regex_t compiled;
  regmatch_t *matches;
  uint64_t n_matches;
  const char *previous_position;
  const char *current_position;
};

// n_matches is the number of expressions expected to match the pattern (so, 1 +
// n_sub_expression)
struct reg_iter reg_iter_new(const char *pattern, const char *to_match,
                             uint64_t n_matches);

/*returns 0 on success (and the new matches will be placed into r.matches)
  or a posix regex error  on error (no more matches, regex error...)*/
int32_t reg_iter_next_match(struct reg_iter *r);

const char *reg_iter_string_for_match(struct reg_iter *r, uint64_t match_index);

void reg_iter_free(struct reg_iter *r);

struct grid {
  uint64_t rows;
  uint64_t cols;
  uint64_t stride;
  char *data;
};

struct grid grid_new(uint64_t rows, uint64_t cols, uint64_t stride);

void *grid_elem(const struct grid *g, uint64_t row, uint64_t col);

void grid_free(const struct grid *g);
