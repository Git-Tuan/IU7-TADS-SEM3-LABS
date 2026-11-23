#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ll_queue.h"

static int show_addr = 0;
static free_address_t freed;
static size_t alloc_total = 0;
static size_t alloc_reused = 0;

void set_show_addr_ll(int flag)
{
    show_addr = flag;
}

void reset_ll_mem_stats(void)
{
    memset(&freed, 0, sizeof(freed));
    alloc_total = 0;
    alloc_reused = 0;
}

void print_ll_mem_stats(void)
{
    printf("Список: выделений памяти %zu, из них адрес совпал с одним из\n"
           "последних %d освобождённых: %zu (%.1f%%)\n",
           alloc_total, MAX_FREE_ADDRESSES, alloc_reused,
           alloc_total ? 100.0 * alloc_reused / alloc_total : 0.0);
}

static void remember_free(ll_queue_t *p)
{
    if (freed.free_count == MAX_FREE_ADDRESSES)
    {
        memmove(freed.free_addresses, freed.free_addresses + 1,
                (MAX_FREE_ADDRESSES - 1) * sizeof(freed.free_addresses[0]));
        freed.free_count--;
    }
    freed.free_addresses[freed.free_count++] = p;
}

static int take_if_freed(ll_queue_t *p)
{
    int found = 0;
    for (size_t i = 0; i < freed.free_count && !found; i++)
    {
        if ((uintptr_t)freed.free_addresses[i] == (uintptr_t)p)
        {
            memmove(freed.free_addresses + i, freed.free_addresses + i + 1,
                    (freed.free_count - i - 1) * sizeof(freed.free_addresses[0]));
            freed.free_count--;
            found = 1;
        }
    }
    return found;
}

rc_t queue_ll(dsc_t *descriptor, int id, double enter_time)
{
    rc_t rc = STATUS_OK;
    ll_queue_t *temp = malloc(sizeof(*temp));

    if (!temp)
        rc = STATUS_MEM_ERR;

    if (rc == STATUS_OK)
    {
        temp->data.id = id;
        temp->data.enter_time = enter_time;
        temp->next = NULL;

        if (descriptor->tail == NULL)
            descriptor->head = temp;
        else
            descriptor->tail->next = temp;
            
        descriptor->tail = temp;
        descriptor->length++;

        int reused = take_if_freed(temp);
        alloc_total++;
        alloc_reused += reused;

        if (show_addr)
            printf("[+] добавлена заявка %5d, адрес %p%s\n", id, (void *)temp,
                   reused ? " (ранее освобождённый адрес)" : " (новый адрес)");
    }

    return rc;
}

rc_t dequeue_ll(dsc_t *descriptor, int *id, double *enter_time)
{
    rc_t rc = STATUS_OK;

    if (descriptor->head == NULL)
        rc = STATUS_POP_ERR;

    if (rc == STATUS_OK)
    {
        ll_queue_t *old = descriptor->head;

        if (id)
            *id = old->data.id;
        if (enter_time)
            *enter_time = old->data.enter_time;

        if (show_addr)
            printf("[-] удалена заявка  %5d, адрес %p (освобождён)\n", old->data.id, (void *)old);

        descriptor->head = old->next;
        if (descriptor->head == NULL)
            descriptor->tail = NULL;
        descriptor->length--;

        remember_free(old);
        free(old);
    }

    return rc;
}

size_t length_ll(const dsc_t *descriptor)
{
    return descriptor->length;
}

void destroy_queue(dsc_t *descriptor)
{
    ll_queue_t *current = descriptor->head;
    while (current != NULL)
    {
        ll_queue_t *nxt = current->next;
        free(current);
        current = nxt;
    }
    descriptor->head = NULL;
    descriptor->tail = NULL;
    descriptor->length = 0;
}
