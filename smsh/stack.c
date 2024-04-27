#include <assert.h>
#include "smsh.h"
#include "stack.h"

int stack_size(struct StackFrame** stack){
  int i = 0;
  while (stack[i] != 0)
      ++i;
  return i;
}

struct StackFrame* stack_pop(struct StackFrame** stack){
  assert(stack_size(stack));
  struct StackFrame* popped_stack_frame = stack[stack_size(stack) - 1];
  stack[stack_size(stack) - 1] = 0; 

  return popped_stack_frame;
}

struct StackFrame* stack_top(struct StackFrame** stack){
  struct StackFrame* popped_stack_frame = stack[stack_size(stack) - 1];
  return popped_stack_frame;
}

void stack_push(struct StackFrame** stack, struct StackFrame* new_stack_frame){
  assert(stack_size(stack) != MAX_STACK_SIZE);
  stack[stack_size(stack)] = new_stack_frame;
}
