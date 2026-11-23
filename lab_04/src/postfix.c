#include <ctype.h>
#include "postfix.h"
#include "static_stack.h"
#include "linked_list_stack.h"
#include "defines.h"

#define UNARY_MINUS '~'

typedef struct
{
    stack_impl_t impl;
    static_stack_t arr;
    linked_stack_t list;
    free_address_t addresses;
} op_stack_t;

typedef struct
{
    char *data;
    size_t size;
    size_t len;
} out_buf_t;

static rc_t op_push(op_stack_t *s, int value)
{
    rc_t rc;

    if (s->impl == STACK_ARRAY)
        rc = push_to_static_stack(&s->arr, value);
    else
        rc = push_to_linked_stack(&s->list, value);

    return rc;
}

static rc_t op_pop(op_stack_t *s, int *value)
{
    rc_t rc;

    if (s->impl == STACK_ARRAY)
        rc = pop_static_stack(&s->arr, value);
    else
        rc = pop_linked_stack(&s->list, value, &s->addresses, 1);

    return rc;
}

static rc_t op_peek(op_stack_t *s, int *value)
{
    rc_t rc;

    if (s->impl == STACK_ARRAY)
        rc = peek_static_stack(&s->arr, value);
    else
        rc = peek_linked_stack(&s->list, value);

    return rc;
}

static rc_t out_append(out_buf_t *buf, char ch)
{
    rc_t rc = STATUS_OK;

    if (buf->len + 1 >= buf->size)
        rc = STATUS_LINE_TOO_LONG;
    else
    {
        buf->data[buf->len] = ch;
        (buf->len)++;
        buf->data[buf->len] = '\0';
    }

    return rc;
}

static rc_t out_append_op(out_buf_t *buf, int op)
{
    rc_t rc = out_append(buf, (char)op);

    if (rc == STATUS_OK)
        rc = out_append(buf, ' ');

    return rc;
}

static int get_priority(char op)
{
    int prior = 0;

    if (op == '+' || op == '-')
        prior = 1;
    else if (op == '*' || op == '/')
        prior = 2;
    else if (op == UNARY_MINUS)
        prior = 3;
    else if (op == '^')
        prior = 4;

    return prior;
}

static int is_operator(char ch)
{
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^';
}

static rc_t process_operand(const char *expr, size_t *pos, out_buf_t *out)
{
    rc_t rc = STATUS_OK;
    size_t start = *pos;
    size_t end;
    int dots = 0;

    while (isdigit((unsigned char)expr[*pos]) || expr[*pos] == '.')
    {
        if (expr[*pos] == '.')
            dots++;
        (*pos)++;
    }
    end = *pos;
    if (dots > 1 || expr[start] == '.' || expr[end - 1] == '.')
        rc = STATUS_EXPR_NUM_ERR;
    for (size_t i = start; rc == STATUS_OK && i < end; i++)
        rc = out_append(out, expr[i]);
    if (rc == STATUS_OK)
        rc = out_append(out, ' ');

    return rc;
}

static rc_t process_operator(op_stack_t *s, char op, out_buf_t *out)
{
    rc_t rc = STATUS_OK;
    int top = 0;
    int stop = 0;

    while (rc == STATUS_OK && !stop && op_peek(s, &top) == STATUS_OK)
    {
        if (get_priority((char)top) > get_priority(op) ||
            (get_priority((char)top) == get_priority(op) && op != '^'))
        {
            rc = op_pop(s, &top);
            if (rc == STATUS_OK)
                rc = out_append_op(out, top);
        }
        else
            stop = 1;
    }
    if (rc == STATUS_OK)
        rc = op_push(s, op);

    return rc;
}

static rc_t process_close_bracket(op_stack_t *s, out_buf_t *out)
{
    rc_t rc = STATUS_OK;
    int top = 0;
    int found = 0;

    while (rc == STATUS_OK && !found)
    {
        rc = op_pop(s, &top);
        if (rc == STATUS_ST_STACK_NOT_SET)
            rc = STATUS_BRACKET_ERR;
        else if (rc == STATUS_OK)
        {
            if (top == '(')
                found = 1;
            else
                rc = out_append_op(out, top);
        }
    }

    return rc;
}

rc_t infix_to_postfix(const char *expr, char *result, size_t size, stack_impl_t impl, free_address_t *freed)
{
    rc_t rc = STATUS_OK;
    op_stack_t stack = {.impl = impl, .list = {.head = NULL, .count = 0}};
    out_buf_t out = {.data = result, .size = size, .len = 0};
    size_t pos = 0;
    int expect_operand = 1;
    int top = 0;
    char ch;

    stack.arr.top = NOT_SET;
    stack.addresses.free_count = 0;
    result[0] = '\0';

    while (rc == STATUS_OK && expr[pos] != '\0')
    {
        ch = expr[pos];
        if (isspace((unsigned char)ch))
            pos++;
        else if (isdigit((unsigned char)ch) || ch == '.')
        {
            if (!expect_operand)
                rc = STATUS_EXPR_SYNTAX_ERR;
            else
            {
                rc = process_operand(expr, &pos, &out);
                expect_operand = 0;
            }
        }
        else if (ch == '(')
        {
            if (!expect_operand)
                rc = STATUS_EXPR_SYNTAX_ERR;
            else
            {
                rc = op_push(&stack, ch);
                pos++;
            }
        }
        else if (ch == ')')
        {
            if (expect_operand)
                rc = STATUS_EXPR_SYNTAX_ERR;
            else
            {
                rc = process_close_bracket(&stack, &out);
                pos++;
            }
        }
        else if (is_operator(ch))
        {
            if (expect_operand && ch == '-')
            {
                rc = op_push(&stack, UNARY_MINUS);
                pos++;
            }
            else if (expect_operand)
                rc = STATUS_EXPR_SYNTAX_ERR;
            else
            {
                rc = process_operator(&stack, ch, &out);
                expect_operand = 1;
                pos++;
            }
        }
        else
            rc = STATUS_EXPR_CHAR_ERR;
    }

    if (rc == STATUS_OK && expect_operand)
        rc = STATUS_EXPR_SYNTAX_ERR;
    while (rc == STATUS_OK && op_pop(&stack, &top) == STATUS_OK)
    {
        if (top == '(')
            rc = STATUS_BRACKET_ERR;
        else
            rc = out_append_op(&out, top);
    }
    if (rc == STATUS_OK && out.len > 0)
        result[out.len - 1] = '\0';

    free_linked_stack(&stack.list, &stack.addresses);
    *freed = stack.addresses;

    return rc;
}
