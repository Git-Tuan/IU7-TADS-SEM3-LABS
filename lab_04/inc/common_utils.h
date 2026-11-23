#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <stddef.h>
#include "status.h"

void flush_input(void);
int is_blank(const char *str);
rc_t read_line(char *buf, size_t size);
rc_t read_int(int *value);
void print_error(rc_t rc);

#endif
