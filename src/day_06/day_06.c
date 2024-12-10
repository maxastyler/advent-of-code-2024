#include "../aoclib.h"
#include "../gen_hashmap.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

struct coord {
  int64_t row;
  int64_t col;
};

struct tile {
  bool obstacle;
  bool visited;
};

GEN_HASHMAP_IMPL(hgrid, struct coord, struct tile,
                 (a->row == b->row) && (a->col == b->col),
                 fnv_hash(fnv_hash(FNV_START, (const char *)&key->row,
                                   sizeof(int64_t)),
                          (const char *)&key->col, sizeof(int64_t)));

struct state {
  uint8_t dir;
  struct coord pos;
  struct hgrid points;
  struct coord bounds;
};

void parse_input(const char *input, struct state *s) {
  struct spliterator lines = spliterator_new(input, "\n");
  const char *line;
  uint64_t row = 0;
  uint64_t n_cols = 0;
  while ((line = spliterator_next(&lines)) != NULL) {
    if (n_cols == 0) {
      n_cols = strlen(line);
    }
    for (uint64_t col = 0; col < strlen(line); col++) {
      if (line[col] == '#') {
        hgrid_insert(&s->points, (struct coord){.row = row, .col = col},
                     (struct tile){.obstacle = true, .visited = false});
      } else if (line[col] == '^') {
        hgrid_insert(&s->points, (struct coord){.row = row, .col = col},
                     (struct tile){.obstacle = false, .visited = true});
        s->pos.row = row;
        s->pos.col = col;
      }
    }
    row++;
  }
  s->bounds.row = row;
  s->bounds.col = n_cols;
  spliterator_free(&lines);
}

bool oob(struct state *s) {
  return s->pos.row < 0 || s->pos.row >= s->bounds.row || s->pos.col < 0 ||
         s->pos.col >= s->bounds.col;
}
// step state, returning false when out of bounds
bool step(struct state *s) {
  hgrid_insert(&s->points, s->pos,
               (struct tile){
                   .obstacle = false,
                   .visited = true,
               });
  struct coord next_pos = s->pos;
  if (s->dir == 0) {
    next_pos.row--;
  } else if (s->dir == 1) {
    next_pos.col++;
  } else if (s->dir == 2) {
    next_pos.row++;
  } else {
    next_pos.col--;
  }
  struct tile *next_tile = hgrid_get(&s->points, next_pos);

  if ((next_tile == NULL) || (next_tile->obstacle == false)) {
    s->pos = next_pos;
  } else {
    s->dir = (s->dir + 1) % 4;
  }
  return !oob(s);
}

void adder(struct coord *c, struct tile *t, void *n) {
  if (t->visited) {
    *(uint64_t *)n += 1;
  }
}

uint64_t part_1(struct state *s) {
  while (step(s)) {
  }
  uint64_t total = 0;
  hgrid_map(&s->points, adder, &total);
  return total;
}

int main(void) {
  const char *input = read_file("src/day_06/input");
  struct state state = {.dir = 0, .points = hgrid_new()};
  parse_input(input, &state);
  printf("Part 1: %lu\n", part_1(&state));
  printf("%lu, %lu\n", state.pos.row, state.pos.col);
}
