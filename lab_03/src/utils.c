/* utils.c */
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

rc_t read_int(const char *prompt, int min_value, int max_value, int *result)
{
    rc_t status = STATUS_STRING_INPUT_ERROR;
    int done = 0;
    int scanned = 0;
    while (!done)
    {
        printf("%s", prompt);
        scanned = scanf("%d", result);
        if (scanned == EOF)
        {
            done = 1;
        }
        else if (scanned != 1)
        {
            printf("  Ошибка: ожидается целое число.\n");
            flush_input();
        }
        else if (*result < min_value || *result > max_value)
        {
            printf("Ошибка: значение должно быть в диапазоне [%d, %d].\n", min_value, max_value);
        }
        else
        {
            done = 1;
            status = STATUS_OK;
        }
    }
    return status;
}

rc_t read_double(const char *prompt, double min_value, double max_value, double *result)
{
    rc_t status = STATUS_STRING_INPUT_ERROR;
    int done = 0;
    int scanned = 0;
    while (!done)
    {
        printf("%s", prompt);
        scanned = scanf("%lf", result);
        if (scanned == EOF)
        {
            done = 1;
        }
        else if (scanned != 1)
        {
            printf("Ошибка: ожидается вещественное число.\n");
            flush_input();
        }
        else if (*result < min_value || *result > max_value)
        {
            printf("Ошибка: значение должно быть в диапазоне [%g, %g].\n", min_value, max_value);
        }
        else
        {
            done = 1;
            status = STATUS_OK;
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
