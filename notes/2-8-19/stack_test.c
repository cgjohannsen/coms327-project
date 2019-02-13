#include <stdio.h>

#include "stack.h"

int main(int argc, char *argv[])
{
  stack_t s;
  int i;

  stack_init(&s);

  for(i = 0; i < 5; i++) {
    stack_push(&s, i);
  }

  stack_peek(&s, &i);
  printf("%d\n", i);

  return 0;
}
