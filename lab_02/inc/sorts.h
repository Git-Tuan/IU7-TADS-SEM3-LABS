#ifndef SORTS_H
#define SORTS_H

#include "toursim_record.h"
#include <stddef.h>

void init_keys(int *keys, size_t n);
void print_keys(int *keys, size_t n);
void table_selection_sort(Record_t *records, size_t rec_len);
void table_quicksort(Record_t *records, size_t low, size_t high);
void key_selection_sort(Record_t *records, int *keys, size_t len);
void keys_quicksort(Record_t *records, int *keys, size_t low, size_t high);

#endif
