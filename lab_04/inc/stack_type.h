#ifndef STACK_TYPE_H
#define STACK_TYPE_H

#include "defines.h"
#include <stddef.h>

typedef struct nodes
{
    int data;
    struct nodes *next;
} node_t;

typedef struct linked_stack
{
    node_t *head;
    size_t count;
} linked_stack_t;

typedef struct static_stack
{
    int stack[MAX_STACK_SIZE];
    int top;
} static_stack_t;

typedef struct free_address
{
    node_t *free_addresses[MAX_FREE_ADDRESSES];
    size_t free_count;
} free_address_t;

#endif
