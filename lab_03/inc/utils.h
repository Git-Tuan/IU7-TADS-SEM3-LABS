/* utils.h */
#ifndef UTILS_H
#define UTILS_H

#include "errors.h"
#include <stddef.h>

void flush_input(void);
rc_t read_int(const char *prompt, int min_value, int max_value, int *result);
rc_t read_double(const char *prompt, double min_value, double max_value, double *result);
rc_t read_int_once(const char *prompt, int *result);
rc_t read_double_once(const char *prompt, double *result);
double get_time_seconds(void);
void print_separator(void);
void random_seed(unsigned int seed);

#endif
