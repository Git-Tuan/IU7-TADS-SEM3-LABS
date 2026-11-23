#ifndef STATIC_STACK_H
#define STATIC_STACK_H

#include "status.h"
#include "stack_type.h"

rc_t peek_static_stack(static_stack_t *st, int *value);
rc_t pop_static_stack(static_stack_t *st, int *popped);
rc_t push_to_static_stack(static_stack_t *st, int value);
void print_static_stack(static_stack_t *st);

#endif
