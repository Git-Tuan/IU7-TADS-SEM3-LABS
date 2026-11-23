#ifndef POSTFIX_H
#define POSTFIX_H

#include <stddef.h>
#include "stack_type.h"
#include "status.h"

typedef enum
{
    STACK_ARRAY,
    STACK_LIST
} stack_impl_t;

rc_t infix_to_postfix(const char *expr, char *result, size_t size, stack_impl_t impl, free_address_t *freed);

#endif
