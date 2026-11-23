#include "common_utils.h"
#include "defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

void flush_input(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {};
}

int is_blank(const char *str)
{
    int blank = 1;

    while (blank && *str != '\0')
    {
        if (!isspace((unsigned char)*str))
            blank = 0;
        str++;
    }

    return blank;
}

rc_t read_line(char *buf, size_t size)
{
    rc_t rc = STATUS_OK;
    size_t len;

    if (fgets(buf, (int)size, stdin) == NULL)
        rc = STATUS_EOF;
    else
    {
        len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        else if (!feof(stdin))
        {
            flush_input();
            rc = STATUS_LINE_TOO_LONG;
        }
    }

    return rc;
}

rc_t read_int(int *value)
{
    char buf[MAX_LINE_LEN];
    char *end;
    long num;
    rc_t rc = read_line(buf, sizeof(buf));

    if (rc == STATUS_OK && is_blank(buf))
        rc = STATUS_EMPTY_INPUT;
    if (rc == STATUS_OK)
    {
        errno = 0;
        num = strtol(buf, &end, 10);
        while (isspace((unsigned char)*end))
            end++;
        if (end == buf || *end != '\0')
            rc = STATUS_GARBAGE_STDIN;
        else if (errno == ERANGE || num > INT_MAX || num < INT_MIN)
            rc = STATUS_INPUT_RANGE_ERR;
        else
            *value = (int)num;
    }

    return rc;
}

void print_error(rc_t rc)
{
    printf("Ошибка: ");
    switch (rc)
    {
        case STATUS_OK:
            printf("ошибки нет\n");
            break;
        case STATUS_ST_PUSH_ERR:
            printf("не удалось добавить элемент в стек\n");
            break;
        case STATUS_PUSH_LIMIT_ERR:
            printf("стек переполнен (максимум элементов: %d)\n", MAX_STACK_SIZE);
            break;
        case STATUS_POP_ERR:
            printf("не удалось исключить элемент из стека\n");
            break;
        case STATUS_ST_STACK_NOT_SET:
            printf("стек пуст, операция невозможна\n");
            break;
        case STATUS_MALLOC_ERR:
            printf("не удалось выделить память\n");
            break;
        case STATUS_MENU_CHOICE_ERR:
            printf("неверный пункт меню, введите число от 0 до %d\n", MENU_MAX_CHOICE);
            break;
        case STATUS_GARBAGE_STDIN:
            printf("нужно ввести целое число без лишних символов\n");
            break;
        case STATUS_EMPTY_INPUT:
            printf("пустой ввод\n");
            break;
        case STATUS_INPUT_RANGE_ERR:
            printf("число вне диапазона [%d; %d]\n", INT_MIN, INT_MAX);
            break;
        case STATUS_LINE_TOO_LONG:
            printf("слишком длинная строка (максимум %d символов)\n", MAX_LINE_LEN - 1);
            break;
        case STATUS_EOF:
            printf("ввод завершен\n");
            break;
        case STATUS_EXPR_SYNTAX_ERR:
            printf("неверный синтаксис выражения (лишний или пропущенный операнд/операция)\n");
            break;
        case STATUS_EXPR_CHAR_ERR:
            printf("недопустимый символ в выражении\n");
            break;
        case STATUS_BRACKET_ERR:
            printf("скобки расставлены неверно\n");
            break;
        case STATUS_EXPR_NUM_ERR:
            printf("неверный формат числа (допустимы вида 12 или 12.5)\n");
            break;
        default:
            printf("неизвестная ошибка (код %d)\n", (int)rc);
            break;
    }
}
