#include "assert.h"
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
