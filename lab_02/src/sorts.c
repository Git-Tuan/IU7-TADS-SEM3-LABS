#include "sorts.h"
#include <stdio.h>

void init_keys(int *keys, size_t n)
{
    for (size_t i = 0; i < n; i++)
        keys[i] = i;
}

void print_keys(int *keys, size_t n)
{
    printf("Ключи: ");
    for (size_t i = 0; i < n; i++)
        printf("%d ", keys[i]);
    printf("\n");
}

static void swap(Record_t *a, Record_t *b)
{
    Record_t temp = *a;
    *a = *b;
    *b = temp;
}

static void keys_swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void table_selection_sort(Record_t *records, size_t rec_len)
{
    for (size_t i = 0; i + 1 < rec_len; i++)
    {
        size_t min_idx = i;

        for (size_t j = i + 1; j < rec_len; j++)
        {
            if (records[j].leisure_cost < records[min_idx].leisure_cost)
                min_idx = j;
        }

        if (min_idx != i)
            swap(&records[i], &records[min_idx]);
    }
}

static size_t table_partition(Record_t *records, size_t low, size_t high)
{
    double pivot = records[high].leisure_cost;
    size_t i = low;

    for (size_t j = low; j < high; j++)
    {
        if (records[j].leisure_cost <= pivot)
        {
            swap(&records[i], &records[j]);
            i++;
        }
    }
    swap(&records[i], &records[high]);
    return i;
}

void table_quicksort(Record_t *records, size_t low, size_t high)
{
    if (low >= high)
        return;

    size_t p = table_partition(records, low, high);

    if (p > low)  table_quicksort(records, low, p - 1);
    if (p < high) table_quicksort(records, p + 1, high);
}

void key_selection_sort(Record_t *records, int *keys, size_t len)
{
    for (size_t i = 0; i + 1 < len; i++)
    {
        size_t min_idx = i;

        for (size_t j = i + 1; j < len; j++)
        {
            if (records[keys[j]].leisure_cost < records[keys[min_idx]].leisure_cost)
                min_idx = j;
        }

        if (min_idx != i)
            keys_swap(&keys[i], &keys[min_idx]);
    }
}

static size_t keys_partition(Record_t *records, int *keys, size_t low, size_t high)
{
    double pivot = records[keys[high]].leisure_cost;
    size_t i = low;

    for (size_t j = low; j < high; j++)
    {
        if (records[keys[j]].leisure_cost <= pivot)
        {
            keys_swap(&keys[i], &keys[j]);
            i++;
        }
    }

    keys_swap(&keys[i], &keys[high]);
    return i;
}

void keys_quicksort(Record_t *records, int *keys, size_t low, size_t high)
{
    if (low >= high)
        return;

    size_t p = keys_partition(records, keys, low, high);

    if (p > low)  keys_quicksort(records, keys, low, p - 1);
    if (p < high) keys_quicksort(records, keys, p + 1, high);
}
