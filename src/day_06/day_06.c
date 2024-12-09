#include "../aoclib.h"
#include "../gen_hashmap.h"
#include "stdbool.h"
#include "stdint.h"

struct coord {
  uint64_t x;
  uint64_t y;
};

struct tile {
  bool visited;
};

GEN_HASHMAP_IMPL(hgrid, struct coord, struct tile,
                 (a->x == b->x) && (a->y == b->y),
                 fnv_hash(fnv_hash(FNV_START, (const char *)&key->x,
                                   sizeof(uint64_t)),
                          (const char *)&key->y, sizeof(uint64_t)));

struct grid parse_input(const char *input) {}

int main(void) { const char *input = read_file("src/day_06/test"); }
