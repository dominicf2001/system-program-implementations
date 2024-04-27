#pragma once

#define MAX_STACK_SIZE 10

int stack_size(struct StackFrame**);
struct StackFrame* stack_pop(struct StackFrame**);
struct StackFrame* stack_top(struct StackFrame**);
void stack_push(struct StackFrame**, struct StackFrame*);
