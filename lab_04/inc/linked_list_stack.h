#ifndef LINKED_LIST_STACK_H
#define LINKED_LIST_STACK_H

#include <stddef.h>
#include "stack_type.h"
#include "status.h"

rc_t peek_linked_stack(linked_stack_t *st, int *value);
rc_t push_to_linked_stack(linked_stack_t *st, int value);
rc_t pop_linked_stack(linked_stack_t *st, int *return_value, free_address_t *addresses, short mode);
rc_t free_linked_stack(linked_stack_t *st, free_address_t *addresses);
void get_linked_stack_state(linked_stack_t *st, free_address_t *addresses);
void print_removed_addresses(free_address_t *addresses);
#endif
