#include <stdio.h>
#include <string.h>
#include "static_queue.h"

static int show_addr = 0;

void set_show_addr_st(int flag)
{
    show_addr = flag;
}

void init_queue_st(static_queue_t *queue)
{
    memset(queue, 0, sizeof(*queue));
}

size_t length_st(const static_queue_t *queue)
{
    return (size_t)queue->count;
}

rc_t queue_st(static_queue_t *queue, int id, double enter_time)
{
    rc_t rc = STATUS_OK;

    if (queue->count >= MAX_QUEUE_SIZE)
        rc = STATUS_PUSH_ERR;

    if (rc == STATUS_OK)
    {
        int pos = (queue->head + queue->count) % MAX_QUEUE_SIZE;

        queue->data[pos].id = id;
        queue->data[pos].enter_time = enter_time;
        queue->tail = (pos + 1) % MAX_QUEUE_SIZE;
        queue->count++;

        if (show_addr)
            printf("[+] добавлена заявка %5d, адрес %p (индекс %d)\n",
                   id, (void *)&queue->data[pos], pos);
    }

    return rc;
}

rc_t queue_pop(static_queue_t *queue, int *id, double *enter_time)
{
    rc_t rc = STATUS_OK;

    if (queue->count == 0)
        rc = STATUS_POP_ERR;

    if (rc == STATUS_OK)
    {
        request_t *el = &queue->data[queue->head];

        if (id)
            *id = el->id;
        if (enter_time)
            *enter_time = el->enter_time;

        if (show_addr)
            printf("[-] удалена заявка  %5d, адрес %p (индекс %d)\n",
                   el->id, (void *)el, queue->head);

        queue->head = (queue->head + 1) % MAX_QUEUE_SIZE;
        queue->count--;
    }

    return rc;
}
