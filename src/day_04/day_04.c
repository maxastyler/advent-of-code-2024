#include "../aoclib.h"
#include "assert.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

uint64_t n_cols(const char *input) {
  uint64_t n = 0;
  while (input[n] != '\n') {
    n++;
  }
  return n;
}
uint64_t n_rows(const char *input) {
  uint64_t n = 0;
  for (uint64_t i = 0; input[i] != '\0'; i++) {
    if (input[i] == '\n') {
      n++;
    }
  }
  return n;
}

struct grid char_grid_from_input_string(const char *input) {
  uint64_t rows = n_rows(input);
  uint64_t cols = n_cols(input);
  struct grid g = grid_new(rows, cols, 1);
  uint64_t gpos = 0;
  for (uint64_t i = 0; i < ((cols + 1) * rows); i++) {
    if (input[i] != '\n') {
      g.data[gpos++] = input[i];
    }
  }
  assert(gpos == rows * cols);
  return g;
}

uint64_t grid_n_xmas_at_position(const struct grid *g, uint32_t row,
                                 uint32_t col) {
  const int8_t row_d[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
  const int8_t col_d[8] = {0, 1, 1, 1, 0, -1, -1, -1};
  const char matches[4] = {'X', 'M', 'A', 'S'};
  uint64_t n_matches = 0;
  for (uint8_t dir = 0; dir < 8; dir++) {
    bool dir_matches = true;
    for (uint8_t i = 0; i < 4; i++) {
      int64_t r = row + (row_d[dir] * i);
      int64_t c = col + (col_d[dir] * i);
      if ((r >= 0) & (c >= 0)) {
        char *current_char = grid_elem(g, r, c);
        if ((current_char == NULL) || (*current_char != matches[i])) {
          dir_matches = false;
          break;
        }
      } else {
        dir_matches = false;
        break;
      }
    }
    if (dir_matches) {
      n_matches++;
    }
  }

  return n_matches;
}

uint64_t part_1(struct grid *g) {
  uint64_t total = 0;
  for (uint64_t row = 0; row < g->rows; row++) {
    for (uint64_t col = 0; col < g->cols; col++) {
      total += grid_n_xmas_at_position(g, row, col);
    }
  }
  return total;
}

bool grid_crossmas_at_position(struct grid *g, uint32_t row, uint32_t col) {
  if ((row == 0) | (col == 0)) {
    return false;
  }
  char *middle = grid_elem(g, row, col);
  if (middle == NULL || *middle != 'A') {
    return false;
  }
  char *tl, *tr, *bl, *br;
  tl = grid_elem(g, row - 1, col - 1);
  tr = grid_elem(g, row - 1, col + 1);
  bl = grid_elem(g, row + 1, col - 1);
  br = grid_elem(g, row + 1, col + 1);
  if (tl == NULL | tr == NULL | bl == NULL | br == NULL) {
    return false;
  }
  return (((*tl == 'M') & (*br == 'S')) | ((*tl == 'S') & (*br == 'M'))) &
         (((*tr == 'M') & (*bl == 'S')) | ((*tr == 'S') & (*bl == 'M')));
}

uint64_t part_2(struct grid *g) {
  uint64_t total = 0;
  for (uint64_t row = 0; row < g->rows; row++) {
    for (uint64_t col = 0; col < g->cols; col++) {
      total += (grid_crossmas_at_position(g, row, col)) ? 1 : 0;
    }
  }
  return total;
}

void run() {
  const char *input = read_file("src/day_04/input");
  struct grid g = char_grid_from_input_string(input);
  free(input);
  printf("Part 1: %lu\n", part_1(&g));
  printf("Part 2: %lu\n", part_2(&g));
  grid_free(&g);
}

int main(void) {
  TIME_FUNCTION_EXECUTION(run, 100);

  return 0;
}
