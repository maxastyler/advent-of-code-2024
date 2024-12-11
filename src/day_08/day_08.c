#include "../aoclib.h"
#include "../gen_hashmap.h"
#include "../gen_vector.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"

struct coord {
  int64_t row;
  int64_t col;
};

GEN_VECTOR_IMPL(pos_vec, struct coord);

GEN_HASHMAP_DEFAULT_IMPL(pos_map, char, struct pos_vec);

GEN_HASHMAP_IMPL(node_map, struct coord, bool,
                 (a->row == b->row) && (a->col == b->col),
                 fnv_hash(fnv_hash(FNV_START, (const char *)&key->row,
                                   sizeof(int64_t)),
                          (const char *)&key->col, sizeof(int64_t)));
struct state {
  struct coord bounds;
  struct pos_map positions;
};

void parse_input(const char *input, struct state *s) {
  s->positions = pos_map_new();
  struct spliterator lines_iter = spliterator_new(input, "\n");
  const char *line;
  struct coord bounds;
  uint64_t row = 0;
  while ((line = spliterator_next(&lines_iter)) != NULL) {

    bounds.col = strlen(line);
    for (uint64_t i = 0; i < bounds.col; i++) {
      if (line[i] != '.') {
        struct pos_vec *p;
        p = pos_map_get(&s->positions, line[i]);
        if (p == NULL) {
          pos_map_insert(&s->positions, line[i], pos_vec_new());
          p = pos_map_get(&s->positions, line[i]);
        }
        pos_vec_push(p, (struct coord){row, i});
      }
    }
    row++;
  }
  bounds.row = row;
  s->bounds = bounds;
}

bool in_bounds(struct coord *c, struct coord *bounds) {
  return (c->row >= 0 && c->col >= 0 && c->row < bounds->row &&
          c->col < bounds->col);
}

void add_nodes(struct coord *a, struct coord *b, struct node_map *ns) {
  int64_t drow = b->row - a->row;
  int64_t dcol = b->col - a->col;
  node_map_insert(ns, (struct coord){b->row + drow, b->col + dcol}, true);
  node_map_insert(ns, (struct coord){a->row - drow, a->col - dcol}, true);
}

void add_nodes_for_pairs(char *key, struct pos_vec *ps, void *ns) {
  for (uint64_t i = 0; i < ps->length; i++) {
    for (uint64_t j = i + 1; j < ps->length; j++) {
      add_nodes(pos_vec_get(ps, i), pos_vec_get(ps, j), ns);
    }
  }
}

struct map_state {
  uint64_t total;
  struct coord bounds;
};

void count_in_bounds(struct coord *k, bool *v, void *map_state) {
  struct map_state *ms = map_state;
  if (in_bounds(k, &ms->bounds)) {
    ms->total++;
  }
}

uint64_t part_1(struct state *s) {
  struct node_map ns = node_map_new();
  pos_map_map(&s->positions, add_nodes_for_pairs, &ns);
  struct map_state ms = {0, s->bounds};
  node_map_map(&ns, count_in_bounds, &ms);
  node_map_free(&ns);

  return ms.total;
}

struct pos_map_state {
  struct state *state;
  struct node_map *nodes;
};

void add_nodes_resonant(struct coord *a, struct coord *b,
                        struct pos_map_state *pms) {
  int64_t drow = b->row - a->row;
  int64_t dcol = b->col - a->col;
  uint64_t i = 0;
  while (true) {
    struct coord c = {b->row + drow * i, b->col + dcol * i};
    if (in_bounds(&c, &pms->state->bounds)) {
      node_map_insert(pms->nodes, c, true);
      i++;
    } else {
      break;
    }
  }
  i = 0;
  while (true) {
    struct coord c = {a->row - drow * i, a->col - dcol * i};
    if (in_bounds(&c, &pms->state->bounds)) {
      node_map_insert(pms->nodes, c, true);
      i++;
    } else {
      break;
    }
  }
}

void add_resonant_nodes_for_pairs(char *key, struct pos_vec *ps, void *pms) {
  for (uint64_t i = 0; i < ps->length; i++) {
    for (uint64_t j = i + 1; j < ps->length; j++) {
      add_nodes_resonant(pos_vec_get(ps, i), pos_vec_get(ps, j), pms);
    }
  }
}

uint64_t part_2(struct state *s) {
  struct node_map ns = node_map_new();
  struct pos_map_state pms = {s, &ns};
  pos_map_map(&s->positions, add_resonant_nodes_for_pairs, &pms);
  struct map_state ms = {0, s->bounds};
  node_map_map(&ns, count_in_bounds, &ms);
  node_map_free(&ns);

  return ms.total;
}

int main(void) {
  const char *input = read_file("src/day_08/input");
  struct state s;
  parse_input(input, &s);
  printf("Part 1: %lu\n", part_1(&s));
  printf("Part 2: %lu\n", part_2(&s));
  return 0;
}
