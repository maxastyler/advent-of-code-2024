#include "../gen_hashmap.h"
#include "stdbool.h"
#include "stdint.h"
#include "../aoclib.h"

struct coord {
  uint64_t x;
  uint64_t y;
};

struct tile {
  bool visited;
};

GEN_HASHMAP_IMPL(hgrid, struct coord, struct tile);

struct grid parse_input(const char* input) {
  
}

int main(void) {
  const char* input = read_file("src/day_06/test");
}
