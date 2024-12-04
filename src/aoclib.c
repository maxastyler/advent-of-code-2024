#include "assert.h"
#include "regex.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

const char *read_file(const char *fname) {
  FILE *f = fopen(fname, "r");
  assert(f != NULL);

  fseek(f, 0L, SEEK_END);
  uint64_t file_length = ftell(f);
  char *buffer = malloc(file_length);
  assert(buffer != NULL);

  rewind(f);

  assert(fread(buffer, 1, file_length, f) == file_length);

  fclose(f);
  return buffer;
}

struct spliterator {
  const char delim;
  char *position;
  char *buffer;
  bool finished;
};

struct spliterator spliterator_new(const char *str, const char delim) {
  size_t buf_len = strlen(str) + 1;
  char *buffer = malloc(buf_len);
  assert(buffer != NULL);
  memcpy(buffer, str, buf_len);
  struct spliterator s = {
      .delim = delim,
      .position = buffer,
      .buffer = buffer,
      .finished = false,
  };
  return s;
}

const char *spliterator_next(struct spliterator *s) {
  if (s->finished) {
    return NULL;
  }
  char *p = s->position;
  while (*p == s->delim) {
    *p = '\0';
    p++;
  }
  char *start_of_split = p;
  while (*p != s->delim) {
    if (*p == '\0') {
      s->finished = true;
      if (p == start_of_split) {
        return NULL;
      } else {
        return start_of_split;
      }
    }
    p++;
  }
  *p = '\0';
  s->position = p + 1;
  return start_of_split;
}

void spliterator_free(struct spliterator *s) {
  free(s->buffer);
  s->buffer = NULL;
  s->position = NULL;
  s->finished = true;
}

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
                             uint64_t n_matches) {
  regmatch_t *matches = malloc(n_matches * sizeof(regmatch_t));
  assert(matches != NULL);
  struct reg_iter r = {
      .matches = matches,
      .n_matches = n_matches,
      .current_position = to_match,
      .previous_position = NULL,
  };
  assert(regcomp(&r.compiled, pattern, REG_EXTENDED) == 0);
  return r;
}

/*returns 0 on success (and the new matches will be placed into r.matches)
  or a posix regex error  on error (no more matches, regex error...)*/
int32_t reg_iter_next_match(struct reg_iter *r) {
  int32_t ret_val =
      regexec(&r->compiled, r->current_position, r->n_matches, r->matches, 0);
  if (ret_val == 0) {
    // match succeeded, increment the current position to end of match
    r->previous_position = r->current_position;
    r->current_position += r->matches[0].rm_eo;
  }
  return ret_val;
}

const char *reg_iter_string_for_match(struct reg_iter *r,
                                      uint64_t match_index) {
  if ((match_index >= r->n_matches) || (r->previous_position == NULL)) {
    return NULL;
  } else {
    return r->previous_position + r->matches[match_index].rm_so;
  }
}

void reg_iter_free(struct reg_iter *r) {
  regfree(&r->compiled);
  free(r->matches);
}

struct grid {
  uint64_t rows;
  uint64_t cols;
  uint64_t stride;
  char *data;
};

struct grid grid_new(uint64_t rows, uint64_t cols, uint64_t stride) {
  char *data = malloc(rows * cols * stride);
  assert(data != NULL);
  return (struct grid){
      .rows = rows,
      .cols = cols,
      .stride = stride,
      .data = data,
  };
}

void *grid_elem(const struct grid *g, uint64_t row, uint64_t col) {
  if ((row < g->rows) & (col < g->cols)) {
    uint64_t index = ((row * g->cols) + col) * g->stride;
    return (g->data + index);
  } else {
    return NULL;
  }
}

void grid_free(const struct grid *g) { free(g->data); }
