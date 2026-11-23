#include <stdio.h>
#include <stdlib.h>
#include "linked_list_stack.h"
#include "defines.h"


rc_t push_to_linked_stack(linked_stack_t *st, int value)
{
    rc_t rc = STATUS_OK;
    node_t *node = NULL;

    if (st->count >= MAX_STACK_SIZE)
        rc = STATUS_PUSH_LIMIT_ERR;
    if (rc == STATUS_OK)
    {
        node = (node_t *)malloc(sizeof(node_t));
        if (!node)
            rc = STATUS_MALLOC_ERR;
    }
    if (rc == STATUS_OK)
    {
        node->next = st->head;
        node->data = value;
        st->head = node;
        (st->count)++;
    }
    return rc;
}

rc_t peek_linked_stack(linked_stack_t *st, int *value)
{
    rc_t rc = STATUS_OK;

    if (st->head != NULL)
        *value = st->head->data;
    else
        rc = STATUS_ST_STACK_NOT_SET;

    return rc;
}

static void add_free_addresses(free_address_t *addresses, node_t *current)
{
    if (addresses->free_count < MAX_FREE_ADDRESSES)
    {
        addresses->free_addresses[addresses->free_count] = (node_t*)current;
        (addresses->free_count)++;
    }
}

rc_t pop_linked_stack(linked_stack_t *st, int *return_value, free_address_t *addresses, short mode)
{
    rc_t rc = STATUS_OK;
    node_t *node = st->head;
    if (st->head == NULL)
    {
        rc = STATUS_ST_STACK_NOT_SET;
    }
    else
    {
        *return_value = node->data;
        if (mode == 1)
            add_free_addresses(addresses, node);
        st->head = node->next;
        (st->count)--;
        free(node);
    }

    return rc;
}

rc_t free_linked_stack(linked_stack_t *st, free_address_t *addresses)
{
    rc_t rc = STATUS_OK;
    node_t *current = st->head;
    
    while (current != NULL)
    {
        node_t *next = current->next;
        add_free_addresses(addresses, current);
        free(current);
        current = next;
    }
    st->head = NULL;
    st->count = 0;

    return rc;
}

void print_removed_addresses(free_address_t *addresses)
{
    printf("Массив освобожденных областей памяти:\n");
    if (addresses->free_count == 0)
    {
        printf("Пока пуст\n");
    }
    else
    {
        printf("+------+--------------------+\n");
        printf("| №    | Адрес              |\n");
        printf("+------+--------------------+\n");
        for (size_t cur_idx = 0; cur_idx < addresses->free_count; cur_idx++)
        {
            printf("| %-4lu | %-18p |\n", (unsigned long)(cur_idx + 1), (void *)addresses->free_addresses[cur_idx]);
        }
        printf("+------+--------------------+\n");
        if (addresses->free_count >= MAX_FREE_ADDRESSES)
            printf("Массив заполнен, новые адреса не записываются\n");
    }
    printf("\n");
}

static void print_stack_data(linked_stack_t *st)
{
    node_t *current = st->head;
    unsigned long num = 1;

    if (st->head == NULL)
    {
        printf("Стек пуст\n");
        return;
    }
    printf("Элементов: %lu из %d, размер узла: %lu байт\n", (unsigned long)st->count,
           MAX_STACK_SIZE, (unsigned long)sizeof(node_t));
    printf("+------+--------------------+--------------+\n");
    printf("| №    | Адрес              | Значение     |\n");
    printf("+------+--------------------+--------------+\n");
    while (current != NULL)
    {
        printf("| %-4lu | %-18p | %12d |%s\n", num, (void *)current, current->data,
               current == st->head ? " <- вершина" : "");
        current = current->next;
        num++;
    }
    printf("+------+--------------------+--------------+\n");
}

void get_linked_stack_state(linked_stack_t *st, free_address_t *addresses)
{
    printf("Текущее состояние стека (связанный список)\n");
    print_stack_data(st);
    print_removed_addresses(addresses);
}
