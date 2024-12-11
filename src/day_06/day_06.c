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
  uint8_t visited;
};

GEN_HASHMAP_IMPL(hgrid, struct coord, struct tile,
                 (a->row == b->row) && (a->col == b->col),
                 fnv_hash(fnv_hash(FNV_START, (const char *)&key->row,
                                   sizeof(int64_t)),
                          (const char *)&key->col, sizeof(int64_t)));

void hgrid_clone_into(struct hgrid *to, const struct hgrid *other) {
  uint64_t len = other->capacity * sizeof(struct hgrid_entry);
  struct hgrid_entry *new_entries = malloc(len);
  assert(new_entries != NULL);
  memcpy(new_entries, other->entries, len);
  memcpy(to, other, sizeof(struct hgrid));
  to->entries = new_entries;
}

struct state {
  uint8_t dir;
  struct coord pos;
  struct hgrid points;
  struct coord bounds;
};

void state_clone_into(struct state *to, const struct state *from) {
  memcpy(to, from, sizeof(struct state));
  hgrid_clone_into(&to->points, &from->points);
}

void state_free(struct state *s) { hgrid_free(&s->points); }

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
                     (struct tile){.obstacle = false, .visited = false});
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
// step state, returning true when tile is revisited with the same direction
bool step(struct state *s) {
  bool revisited = false;
  struct tile *current = hgrid_get(&s->points, s->pos);
  if (current != NULL) {
    /* printf("%d, %d\n", s->dir, current->visited); */
    if (((1 << s->dir) & (current->visited)) != 0) {
      revisited = true;
    }
    current->visited |= 1 << s->dir;
  } else {
    hgrid_insert(&s->points, s->pos,
                 (struct tile){
                     .obstacle = false,
                     .visited = 1 << s->dir,
                 });
  }
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
  return revisited;
}

void adder(struct coord *c, struct tile *t, void *n) {
  if (t->visited != 0) {
    *(uint64_t *)n += 1;
  }
}

uint64_t part_1(const struct state *s_from) {
  struct state s;
  state_clone_into(&s, s_from);
  do {
    step(&s);
  } while (!oob(&s));
  uint64_t total = 0;
  hgrid_map(&s.points, adder, &total);
  state_free(&s);
  return total;
}

// returns true if a block at the given position would make the guard loop
bool part_2_at_position(const struct state *initial, int64_t row, int64_t col) {
  bool retval = false;
  struct state s;
  state_clone_into(&s, initial);
  hgrid_insert(&s.points, (struct coord){row, col}, (struct tile){true, 0});
  do {
    if (step(&s)) {
      retval = true;
      break;
    }
  } while (!oob(&s));
  state_free(&s);
  return retval;
}

uint64_t part_2(const struct state *s_from) {
  uint64_t total = 0;
  for (int64_t row = 0; row < s_from->bounds.row; row++) {
    for (int64_t col = 0; col < s_from->bounds.col; col++) {
      if (!(s_from->pos.row == row && s_from->pos.col == col)) {
        if (part_2_at_position(s_from, row, col)) {
          total++;
        };
      }
    }
  }
  return total;
}

int main(void) {
  const char *input = read_file("src/day_06/input");
  struct state state = {.dir = 0, .points = hgrid_new()};
  parse_input(input, &state);
  printf("Part 1: %lu\n", part_1(&state));
  printf("Part 2: %lu\n", part_2(&state));
}
