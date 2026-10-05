#include "measure_sorts.h"
#include "sorts.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MEASURE_REPEAT 1000

static double measure_table_sort(void (*sort_fn)(Record_t*, size_t), Record_t *original, size_t n)
{
    Record_t copy[MAX_RECORD_AMOUNT];
    double total = 0.0;

    for (int k = 0; k < MEASURE_REPEAT; k++)
    {
        memcpy(copy, original, n * sizeof(Record_t));

        clock_t start = clock();
        sort_fn(copy, n);
        clock_t end = clock();

        total += (double)(end - start) / CLOCKS_PER_SEC;
    }

    return total / MEASURE_REPEAT;
}

static double measure_table_quick(Record_t *original, size_t n)
{
    Record_t copy[MAX_RECORD_AMOUNT];
    double total = 0.0;

    for (int k = 0; k < MEASURE_REPEAT; k++)
    {
        memcpy(copy, original, n * sizeof(Record_t));

        clock_t start = clock();
        table_quicksort(copy, 0, n - 1);
        clock_t end = clock();

        total += (double)(end - start) / CLOCKS_PER_SEC;
    }

    return total / MEASURE_REPEAT;
}


static double measure_keys_selection(Record_t *original, size_t n)
{
    int keys[MAX_RECORD_AMOUNT];
    double total = 0.0;

    for (int k = 0; k < MEASURE_REPEAT; k++)
    {
        init_keys(keys, n);

        clock_t start = clock();
        key_selection_sort(original, keys, n);
        clock_t end = clock();

        total += (double)(end - start) / CLOCKS_PER_SEC;
    }

    return total / MEASURE_REPEAT;
}

static double measure_keys_quick(Record_t *original, size_t n)
{
    int keys[MAX_RECORD_AMOUNT];
    double total = 0.0;

    for (int k = 0; k < MEASURE_REPEAT; k++)
    {
        init_keys(keys, n);

        clock_t start = clock();
        keys_quicksort(original, keys, 0, n - 1);
        clock_t end = clock();

        total += (double)(end - start) / CLOCKS_PER_SEC;
    }

    return total / MEASURE_REPEAT;
}

static double percent_faster(double y, double x)
{
    if (y <= 0.0) return 0.0;
    return 100.0 * (y - x) / y;
}

rc_t measure_sorts(Record_t *records, size_t rec_len)
{
    if (records == NULL)
        return STATUS_NULL_POINTER_ERR;

    if (rec_len < 2)
    {
        printf("Недостаточно записей для сравнения сортировок\n");
        return STATUS_NO_ERROR;
    }

    printf("Идёт замер, пожалуйста подождите... (%d прогонов на алгоритм)\n",
           MEASURE_REPEAT);

    double t_table_sel = measure_table_sort(table_selection_sort, records, rec_len);
    double t_table_qs  = measure_table_quick(records, rec_len);
    double t_keys_sel  = measure_keys_selection(records, rec_len);
    double t_keys_qs   = measure_keys_quick(records, rec_len);

    size_t table_mem = sizeof(Record_t) * rec_len;
    size_t keys_mem  = sizeof(int)      * rec_len;

    printf("\nСравнение эффективности сортировок\n");
    printf("Количество записей: %zu\n", rec_len);

    printf("\nВремя (секунды):\n");
    printf("Таблица | выборная : %10.8f\n", t_table_sel);
    printf("Таблица | быстрая  : %10.8f\n", t_table_qs);
    printf("Ключи   | выборная : %10.8f\n", t_keys_sel);
    printf("Ключи   | быстрая  : %10.8f\n", t_keys_qs);

    printf("\nПамять:\n");
    printf("Таблица : %8zu байт (100.0%%)\n", table_mem);
    printf("Ключи   : %8zu байт (%.2f%% от таблицы)\n", keys_mem, 100.0 * keys_mem / table_mem);

    printf("\nОтносительное ускорение:\n");
    printf("Быстрая сортировка таблицы vs выборная : %.1f%% быстрее\n",
           percent_faster(t_table_sel, t_table_qs));
    printf("Быстрая сортировка ключей  vs выборная : %.1f%% быстрее\n",
           percent_faster(t_keys_sel, t_keys_qs));

    printf("Ключи vs таблица (обе выборные)        : %.1f%% быстрее\n",
           percent_faster(t_table_sel, t_keys_sel));
    printf("Ключи vs таблица (обе быстрые)         : %.1f%% быстрее\n",
           percent_faster(t_table_qs, t_keys_qs));

    printf("\nОбщая эффективность:\n");
    double total_table = t_table_sel + t_table_qs;
    double total_keys  = t_keys_sel  + t_keys_qs;

    printf("Суммарное время (таблица) : %.8f с\n", total_table);
    printf("Суммарное время (ключи)   : %.8f с\n", total_keys);
    printf("Использование ключей даёт : %.1f%% экономии времени\n",
           percent_faster(total_table, total_keys));
    printf("Цена по памяти            : +%.2f%% (массив ключей)\n",
           100.0 * keys_mem / table_mem);

    return STATUS_NO_ERROR;
}
