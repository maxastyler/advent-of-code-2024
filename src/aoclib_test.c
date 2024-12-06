#include "aoclib.h"
#include "assert.h"
#include "stdio.h"
#include "string.h"

int test_spliterator() {
  struct spliterator s = spliterator_new("\nmy big\n string\n", "\n");
  assert(strcmp(spliterator_next(&s), "my big") == 0);
  const char *s_next = spliterator_next(&s);
  printf("The string is: %s\n", s_next);
  assert(strcmp(s_next, " string") == 0);
  assert(spliterator_next(&s) == NULL);
  assert(spliterator_next(&s) == NULL);
  return 0;
}

int test_spliterator_multiple() {
  struct spliterator s = spliterator_new("\n\nmy big\n\n string\n", "\n\n");
  assert(strcmp(spliterator_next(&s), "my big") == 0);
  const char *s_next = spliterator_next(&s);
  assert(strcmp(s_next, " string\n") == 0);
  assert(spliterator_next(&s) == NULL);
  assert(spliterator_next(&s) == NULL);
  return 0;
}

int main(void) {
  /* if (test_spliterator() != 0) { */
  /*   return 1; */
  /* } */
  if (test_spliterator_multiple() != 0) {
    return 1;
  }  
  return 0;
}
