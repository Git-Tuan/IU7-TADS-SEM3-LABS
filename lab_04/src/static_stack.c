#include <stdio.h>
#include "static_stack.h"
#include "defines.h"

rc_t peek_static_stack(static_stack_t *st, int *value)
{
    rc_t rc = STATUS_OK;

    if (st->top != NOT_SET)
    {
        *value = st->stack[st->top];
    }
    else
    {
        rc = STATUS_ST_STACK_NOT_SET;
    }

    return rc;
}

rc_t pop_static_stack(static_stack_t *st, int *popped)
{
    rc_t rc = STATUS_OK;

    if (st->top != NOT_SET)
    {
        *popped = st->stack[st->top];
        (st->top)--;
    }
    else
    {
        rc = STATUS_ST_STACK_NOT_SET;
    }

    return rc;
}

rc_t push_to_static_stack(static_stack_t *st, int value)
{
    rc_t rc = STATUS_OK;

    if (st->top >= MAX_STACK_SIZE - 1)
        rc = STATUS_PUSH_LIMIT_ERR;
    if (rc == STATUS_OK)
    {
        (st->top)++;
        st->stack[st->top] = value;
    }

    return rc;
}

void print_static_stack(static_stack_t *st)
{
    printf("Текущее состояние стека (массив)\n");
    if (st->top == NOT_SET)
    {
        printf("Стек пуст\n");
        return;
    }
    printf("Элементов: %d из %d\n", st->top + 1, MAX_STACK_SIZE);
    printf("+--------+--------------+\n");
    printf("| Индекс | Значение     |\n");
    printf("+--------+--------------+\n");
    for (int i = st->top; i >= 0; i--)
    {
        printf("| %6d | %12d |%s\n", i, st->stack[i], i == st->top ? " <- вершина" : "");
    }
    printf("+--------+--------------+\n");
}
