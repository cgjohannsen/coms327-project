#ifndef STACK_H
# define STACK_H

typedef struct stack_node {
  int value;
  struct stack_node *next;
} stack_node_t;

typedef struct stack {
  stack_node_t *top;
  int sizw;
} stack_t;

int stack_init(stack_t *s);
int stack_delete(stack_t *s);
int stack_peak(stack_t *s, int *value);
int stack_push(stack_t *s, int value);
int stack_pop(stack_t *s, int *value);
int stack_size(stack_t *s);
int stack_is_empty(stack_t *s);

#endif
