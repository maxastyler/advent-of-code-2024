#pragma once

#include "stdbool.h"

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
