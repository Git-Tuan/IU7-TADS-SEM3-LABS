#include "menu.h"
#include "stack_type.h"
#include "static_stack.h"
#include "linked_list_stack.h"
#include "common_utils.h"
#include "postfix.h"
#include "benchmark.h"
#include "defines.h"
#include <stdio.h>
#include <limits.h>

static void handle_user_choice(static_stack_t *st, linked_stack_t *ls, free_address_t *addresses, int choice);
static void push_static(static_stack_t *st);
static void pop_static(static_stack_t *st);
static void push_linked(linked_stack_t *ls);
static void pop_linked(linked_stack_t *ls, free_address_t *addresses);
static void convert_expression(stack_impl_t impl);

rc_t menu(void)
{
    rc_t rc = STATUS_OK;
    static_stack_t stack_context = {.top = NOT_SET};
    int choice = -1;
    linked_stack_t linked_context = {.head = NULL, .count = 0};
    free_address_t address_context = {.free_count = 0};

    while (choice != 0)
    {
        rc = STATUS_OK;
        printf("\n========== Работа со стеком (максимум элементов: %d) ==========\n", MAX_STACK_SIZE);
        printf("0) Выход\n");
        printf("1) Вывести стек (версия - массив)\n");
        printf("2) Добавить элемент в стек (реализация - массив)\n");
        printf("3) Исключить элемент из стека (реализация - массив)\n");
        printf("4) Вывести стек (версия - связанный список)\n");
        printf("5) Добавить элемент в стек (реализация - связанный список)\n");
        printf("6) Исключить элемент из стека (реализация - связанный список)\n");
        printf("7) Вывести массив адресов удаленных элементов\n");
        printf("8) Перевести выражение в постфиксную форму (стек - массив)\n");
        printf("9) Перевести выражение в постфиксную форму (стек - связанный список)\n");
        printf("10) Сравнить время и память (массив и связанный список)\n");
        printf("Ваш выбор [0-%d]: ", MENU_MAX_CHOICE);
        rc = read_int(&choice);
        if (rc == STATUS_EOF)
        {
            choice = 0;
            rc = STATUS_OK;
        }
        else if (rc == STATUS_OK && (choice < 0 || choice > MENU_MAX_CHOICE))
            rc = STATUS_MENU_CHOICE_ERR;
        if (rc == STATUS_OK)
            handle_user_choice(&stack_context, &linked_context, &address_context, choice);
        else
            print_error(rc);
    }
    free_linked_stack(&linked_context, &address_context);

    return rc;
}

static void handle_user_choice(static_stack_t *st, linked_stack_t *ls, free_address_t *addresses, int choice)
{
    switch(choice)
    {
        case 0:
            break;
        case 1:
            print_static_stack(st);
            break;
        case 2:
            push_static(st);
            break;
        case 3:
            pop_static(st);
            break;
        case 4:
            get_linked_stack_state(ls, addresses);
            break;
        case 5:
            push_linked(ls);
            break;
        case 6:
            pop_linked(ls, addresses);
            break;
        case 7:
            print_removed_addresses(addresses);
            break;
        case 8:
            convert_expression(STACK_ARRAY);
            break;
        case 9:
            convert_expression(STACK_LIST);
            break;
        case 10:
            compare_stacks();
            break;
        default:
            break;
    }
}

static void push_static(static_stack_t *st)
{
    int value = 0;
    rc_t rc;

    printf("Добавление элемента в стек (массив).\n");
    printf("Введите целое число [%d; %d]: ", INT_MIN, INT_MAX);
    rc = read_int(&value);
    if (rc == STATUS_OK)
        rc = push_to_static_stack(st, value);
    if (rc == STATUS_OK)
        printf("Элемент %d добавлен в стек\n", value);
    else
        print_error(rc);
}

static void pop_static(static_stack_t *st)
{
    int value = 0;
    rc_t rc = pop_static_stack(st, &value);

    printf("Исключение элемента из стека (массив).\n");
    if (rc == STATUS_OK)
        printf("Из стека исключен элемент: %d\n", value);
    else
        print_error(rc);
}

static void push_linked(linked_stack_t *ls)
{
    int value = 0;
    rc_t rc;

    printf("Добавление элемента в стек (связанный список).\n");
    printf("Введите целое число [%d; %d]: ", INT_MIN, INT_MAX);
    rc = read_int(&value);
    if (rc == STATUS_OK)
        rc = push_to_linked_stack(ls, value);
    if (rc == STATUS_OK)
        printf("Элемент %d добавлен в стек, адрес узла: %p\n", value, (void *)ls->head);
    else
        print_error(rc);
}

static void pop_linked(linked_stack_t *ls, free_address_t *addresses)
{
    int value = 0;
    rc_t rc;

    printf("Исключение элемента из стека (связанный список).\n");
    rc = pop_linked_stack(ls, &value, addresses, 1);
    if (rc == STATUS_OK)
        printf("Из стека исключен элемент: %d\nПамять узла освобождена, адрес записан в массив (пункт 7)\n", value);
    else
        print_error(rc);
}

static void convert_expression(stack_impl_t impl)
{
    char expr[MAX_LINE_LEN];
    char result[2 * MAX_LINE_LEN];
    free_address_t freed = {.free_count = 0};
    rc_t rc;

    printf("Перевод в постфиксную форму (стек операций - %s).\n", impl == STACK_ARRAY ? "массив" : "связанный список");
    printf("Операнды: числа (12 или 12.5), операции: + - * / ^, унарный минус, скобки: ( ).\n");
    printf("Введите выражение (до %d символов): ", MAX_LINE_LEN - 1);
    rc = read_line(expr, sizeof(expr));
    if (rc == STATUS_OK && is_blank(expr))
        rc = STATUS_EMPTY_INPUT;
    if (rc == STATUS_OK)
        rc = infix_to_postfix(expr, result, sizeof(result), impl, &freed);
    if (rc == STATUS_OK)
    {
        printf("Постфиксная запись: %s\n", result);
        printf("(знак ~ в записи означает унарный минус)\n");
        if (impl == STACK_LIST)
            print_removed_addresses(&freed);
    }
    else
        print_error(rc);
}
