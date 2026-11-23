#ifndef LL_QUEUE_H
#define LL_QUEUE_H

#include <stddef.h>
#include "status.h"
#include "queue_type.h"

rc_t queue_ll(dsc_t *descriptor, int id, double enter_time);
rc_t dequeue_ll(dsc_t *descriptor, int *id, double *enter_time);
size_t length_ll(const dsc_t *descriptor);
void destroy_queue(dsc_t *descriptor);

void set_show_addr_ll(int flag);
void reset_ll_mem_stats(void);
void print_ll_mem_stats(void);

#endif
