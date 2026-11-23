#include <stdio.h>
#include <time.h>
#include "benchmark.h"
#include "static_stack.h"
#include "linked_list_stack.h"
#include "defines.h"

static double bench_static_stack(void)
{
    static_stack_t st = {.top = NOT_SET};
    int value = 0;
    clock_t start = clock();

    for (int rep = 0; rep < BENCH_REPEATS; rep++)
    {
        for (int i = 0; i < BENCH_STACK_ELEMENTS; i++)
            push_to_static_stack(&st, i);
        for (int i = 0; i < BENCH_STACK_ELEMENTS; i++)
            pop_static_stack(&st, &value);
    }

    return (double)(clock() - start) * 1000.0 / CLOCKS_PER_SEC;
}

static double bench_linked_stack(void)
{
    linked_stack_t ls = {.head = NULL, .count = 0};
    free_address_t addresses = {.free_count = 0};
    int value = 0;
    clock_t start = clock();

    for (int rep = 0; rep < BENCH_REPEATS; rep++)
    {
        for (int i = 0; i < BENCH_STACK_ELEMENTS; i++)
            push_to_linked_stack(&ls, i);
        for (int i = 0; i < BENCH_STACK_ELEMENTS; i++)
            pop_linked_stack(&ls, &value, &addresses, 0);
    }

    return (double)(clock() - start) * 1000.0 / CLOCKS_PER_SEC;
}

static void print_time_table(void)
{
    double t_arr = bench_static_stack();
    double t_list = bench_linked_stack();
    double ops = 2.0 * BENCH_STACK_ELEMENTS * BENCH_REPEATS;

    printf("\nВремя: %d циклов \"заполнить %d элементов - опустошить стек\"\n", BENCH_REPEATS, BENCH_STACK_ELEMENTS);
    printf("+--------------+--------------+-----------------+\n");
    printf("| Реализация   | Время, мс    | Нс на операцию  |\n");
    printf("+--------------+--------------+-----------------+\n");
    printf("| Массив       | %12.3f | %15.1f |\n", t_arr, t_arr * 1e6 / ops);
    printf("| Список       | %12.3f | %15.1f |\n", t_list, t_list * 1e6 / ops);
    printf("+--------------+--------------+-----------------+\n");
    if (t_arr > 0)
        printf("Список: %.1f %% от времени массива (разница %+.1f %%)\n", t_list / t_arr * 100.0,
               (t_list - t_arr) / t_arr * 100.0);
    else
        printf("Время массива слишком мало для измерения\n");
}

static void print_memory_table(void)
{
    size_t arr_size = sizeof(static_stack_t);
    size_t node_size = sizeof(node_t);
    size_t desc_size = sizeof(linked_stack_t);

    printf("\nПамять (без служебных данных менеджера памяти)\n");
    printf("+-----------+--------------+--------------+------------------+\n");
    printf("| Элементов | Массив, байт | Список, байт | Список/массив, %% |\n");
    printf("+-----------+--------------+--------------+------------------+\n");

    size_t max_lines = 50;
    size_t step = (BENCH_STACK_ELEMENTS + max_lines - 1) / max_lines;
    if (step < 1) step = 1;

    for (size_t n = 1; n <= BENCH_STACK_ELEMENTS; n += step)
    {
        printf("| %9lu | %12lu | %12lu | %16.1f |\n", (unsigned long)n, (unsigned long)arr_size,
               (unsigned long)(n * node_size + desc_size), (double)(n * node_size + desc_size) / arr_size * 100.0);
    }

    if ((BENCH_STACK_ELEMENTS - 1) % step != 0) 
    {
        size_t n = BENCH_STACK_ELEMENTS;
        printf("| %9lu | %12lu | %12lu | %16.1f |\n", (unsigned long)n, (unsigned long)arr_size,
               (unsigned long)(n * node_size + desc_size), (double)(n * node_size + desc_size) / arr_size * 100.0);
    }

    printf("+-----------+--------------+--------------+------------------+\n");
    printf("Узел списка: %lu байт, дескриптор списка: %lu байт (включен в итог), массив занимает %lu байт независимо от заполнения\n",
           (unsigned long)node_size, (unsigned long)desc_size, (unsigned long)arr_size);
}

void compare_stacks(void)
{
    print_time_table();
    print_memory_table();
}
