#ifndef NUMBERS_IO_H
#define NUMBERS_IO_H

#include "calc_functions.h"
#include "parse_functions.h"

void multiply_integer_by_float(const integer_t *a, const floating_point_t *b, long_number_t *result);
return_code_t print_result_from(const integer_t *a, const floating_point_t *b, const long_number_t *result);
return_code_t read_float(floating_point_t *out);
return_code_t read_integer(integer_t *out);

#endif
