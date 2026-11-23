/* utils.c */
#include "utils.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INPUT_BUF_SIZE 128
#define PROMPT_BUF_SIZE 256

void flush_input(void)
{
    int ch = 0;
    int done = 0;
    while (!done)
    {
        ch = getchar();
        if (ch == '\n' || ch == EOF)
        {
            done = 1;
        }
    }
}

static rc_t read_line(char *buf, size_t size)
{
    rc_t status = STATUS_OK;
    if (fgets(buf, (int)size, stdin) == NULL)
    {
        status = STATUS_INPUT_EOF;
    }
    else
    {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
        {
            buf[len - 1] = '\0';
        }
        else if (len == size - 1)
        {
            flush_input();
            status = STATUS_STRING_INPUT_ERROR;
        }
    }
    return status;
}

static int only_spaces_left(const char *s)
{
    while (isspace((unsigned char)*s))
    {
        s = s + 1;
    }
    return *s == '\0';
}

rc_t read_int_once(const char *prompt, int *result)
{
    char buf[INPUT_BUF_SIZE];
    char *end = NULL;
    long value = 0;
    rc_t status = STATUS_OK;

    printf("%s", prompt);
    status = read_line(buf, sizeof(buf));
    if (status == STATUS_OK)
    {
        errno = 0;
        value = strtol(buf, &end, 10);
        if (end == buf || errno == ERANGE || value < INT_MIN || value > INT_MAX || !only_spaces_left(end))
        {
            status = STATUS_STRING_INPUT_ERROR;
        }
        else
        {
            *result = (int)value;
        }
    }
    return status;
}

rc_t read_double_once(const char *prompt, double *result)
{
    char buf[INPUT_BUF_SIZE];
    char *end = NULL;
    double value = 0.0;
    rc_t status = STATUS_OK;

    printf("%s", prompt);
    status = read_line(buf, sizeof(buf));
    if (status == STATUS_OK)
    {
        errno = 0;
        value = strtod(buf, &end);
        if (end == buf || errno == ERANGE || !isfinite(value) || !only_spaces_left(end))
        {
            status = STATUS_STRING_INPUT_ERROR;
        }
        else
        {
            *result = value;
        }
    }
    return status;
}

static void make_range_prompt(char *out, size_t size, const char *prompt, const char *range)
{
    size_t len = strlen(prompt);
    while (len > 0 && (prompt[len - 1] == ' ' || prompt[len - 1] == ':'))
    {
        len = len - 1;
    }
    snprintf(out, size, "%.*s [%s]: ", (int)len, prompt, range);
}

rc_t read_int(const char *prompt, int min_value, int max_value, int *result)
{
    rc_t status = STATUS_STRING_INPUT_ERROR;
    int done = 0;
    int value = 0;
    char range[64];
    char full_prompt[PROMPT_BUF_SIZE];
    snprintf(range, sizeof(range), "%d..%d", min_value, max_value);
    make_range_prompt(full_prompt, sizeof(full_prompt), prompt, range);
    while (!done)
    {
        status = read_int_once(full_prompt, &value);
        if (status == STATUS_INPUT_EOF)
        {
            done = 1;
        }
        else if (status != STATUS_OK)
        {
            printf("Ошибка: ожидается целое число.\n");
        }
        else if (value < min_value || value > max_value)
        {
            printf("Ошибка: значение должно быть в диапазоне [%d, %d].\n", min_value, max_value);
        }
        else
        {
            *result = value;
            done = 1;
        }
    }
    return status;
}

rc_t read_double(const char *prompt, double min_value, double max_value, double *result)
{
    rc_t status = STATUS_STRING_INPUT_ERROR;
    int done = 0;
    double value = 0.0;
    char range[64];
    char full_prompt[PROMPT_BUF_SIZE];
    snprintf(range, sizeof(range), "%g..%g", min_value, max_value);
    make_range_prompt(full_prompt, sizeof(full_prompt), prompt, range);
    while (!done)
    {
        status = read_double_once(full_prompt, &value);
        if (status == STATUS_INPUT_EOF)
        {
            done = 1;
        }
        else if (status != STATUS_OK)
        {
            printf("Ошибка: ожидается вещественное число.\n");
        }
        else if (value < min_value || value > max_value)
        {
            printf("Ошибка: значение должно быть в диапазоне [%g, %g].\n", min_value, max_value);
        }
        else
        {
            *result = value;
            done = 1;
        }
    }
    return status;
}

double get_time_seconds(void)
{
    return (double)clock() / (double)CLOCKS_PER_SEC;
}

void print_separator(void)
{
    printf("------------------------------------------------------------\n");
}

void random_seed(unsigned int seed)
{
    srand(seed);
}
