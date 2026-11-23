#ifndef STACK_TYPE_H
#define STACK_TYPE_H

#include "defines.h"
#include <stddef.h>

typedef struct request
{
    int id;
    double enter_time;
} request_t;

typedef struct static_queue
{
    request_t data[MAX_QUEUE_SIZE];
    int head;
    int tail;
    int count;
} static_queue_t;

typedef struct ll_queue
{
    request_t data;
    struct ll_queue *next;
} ll_queue_t;

typedef struct descriptor
{
    ll_queue_t *head;
    ll_queue_t *tail;
    size_t length;
} dsc_t;

typedef struct free_address
{
    ll_queue_t *free_addresses[MAX_FREE_ADDRESSES];
    size_t free_count;
} free_address_t;

#endif
