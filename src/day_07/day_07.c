#include "../aoclib.h"
#include "../gen_vector.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"

GEN_VECTOR_IMPL(ivec, uint64_t);

struct eq {
  uint64_t result;
  struct ivec values;
};

GEN_VECTOR_IMPL(eq_vec, struct eq);

struct res {
  bool valid;
  uint64_t value;
};

void parse_line(const char *line, struct eq *eq) {
  struct spliterator l = spliterator_new(line, ": ");
  const char *num = spliterator_next(&l);
  eq->result = strtol(num, NULL, 10);
  struct spliterator rest = spliterator_new(spliterator_next(&l), " ");
  struct ivec values = ivec_new();
  const char *v;
  while ((v = spliterator_next(&rest)) != NULL) {
    ivec_push(&values, strtol(v, NULL, 10));
  }
  eq->values = values;
  spliterator_free(&rest);
  spliterator_free(&l);
}

struct eq_vec parse_input(const char *input) {
  struct eq_vec v = eq_vec_new();
  struct spliterator lines_iter = spliterator_new(input, "\n");
  const char *line;
  while ((line = (spliterator_next(&lines_iter))) != NULL) {
    struct eq eq;
    parse_line(line, &eq);
    eq_vec_push(&v, eq);
  }
  return v;
}

bool p1_sat(uint64_t target, uint64_t current, struct ivec_slice rest) {
  if (rest.length == 0) {
    return target == current;
  } else if (current > target) {
    return false;
  } else {
    uint64_t v = rest.buffer[0];
    return (p1_sat(target, current + v,
                   (struct ivec_slice){rest.buffer + 1, rest.length - 1}) ||
            p1_sat(target, current * v,
                   (struct ivec_slice){rest.buffer + 1, rest.length - 1}));
  }
}

uint64_t count_decimal_places(uint64_t n) {
  uint64_t n_places = 0;
  do {
    n_places++;
  } while ((n /= 10) > 0);
  return n_places;
}

uint64_t combine(uint64_t a, uint64_t b) {
  for (uint8_t i = 0; i < count_decimal_places(b); i++) {
    a *= 10;
  }
  return a + b;
}

bool p2_sat(uint64_t target, uint64_t current, struct ivec_slice rest) {
  /* printf("%lu, %lu, %lu\n", target, current, rest.length); */
  if (rest.length == 0) {
    return target == current;
  } else if (current > target) {
    return false;
  } else {
    uint64_t v = rest.buffer[0];
    return (p2_sat(target, current + v,
                   (struct ivec_slice){rest.buffer + 1, rest.length - 1}) ||
            p2_sat(target, current * v,
                   (struct ivec_slice){rest.buffer + 1, rest.length - 1}) ||
            p2_sat(target, combine(current, v),
                   (struct ivec_slice){rest.buffer + 1, rest.length - 1}));
  }
}

uint64_t part_1(const struct eq_vec *eqs) {
  uint64_t total = 0;
  for (uint64_t i = 0; i < eqs->length; i++) {
    if (p1_sat(eqs->buffer[i].result, eqs->buffer[i].values.buffer[0],
               ivec_get_slice(&eqs->buffer[i].values, 1,
                              eqs->buffer[i].values.length))) {
      total += eqs->buffer[i].result;
    };
  }
  return total;
}

uint64_t part_2(const struct eq_vec *eqs) {
  uint64_t total = 0;
  for (uint64_t i = 0; i < eqs->length; i++) {
    if (p2_sat(eqs->buffer[i].result, eqs->buffer[i].values.buffer[0],
               ivec_get_slice(&eqs->buffer[i].values, 1,
                              eqs->buffer[i].values.length))) {
      total += eqs->buffer[i].result;
    };
  }
  return total;
}

int main(void) {
  const char *input = read_file("src/day_07/input");
  struct eq_vec eqs = parse_input(input);
  printf("Part 1: %lu\n", part_1(&eqs));
  printf("Part 2: %lu\n", part_2(&eqs));
  return 0;
}
