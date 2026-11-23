#ifndef STATIC_QUEUE_H
#define STATIC_QUEUE_H

#include <stddef.h>
#include "status.h"
#include "queue_type.h"

void init_queue_st(static_queue_t *queue);
rc_t queue_st(static_queue_t *queue, int id, double enter_time);
rc_t queue_pop(static_queue_t *queue, int *id, double *enter_time);
size_t length_st(const static_queue_t *queue);
void set_show_addr_st(int flag);

#endif
