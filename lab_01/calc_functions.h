#ifndef CALC_FUNCTIONS_H
#define CALC_FUNCTIONS_H
#include "defines.h"
#include "status.h"
#include <stddef.h>

typedef struct
{
    int tetrads[MAX_RESULT_TETRADS];
    size_t len;
    int exp;
} long_number_t;

return_code_t convert_numerical_string_to_int(const char *str, int *dest, size_t *dest_len);
void multiply_long_numbers(const long_number_t *a, const long_number_t *b, long_number_t *result);
void tetrads_to_string(char *mant_str, const int *tetrads, size_t len);
void round_mantissa(char *mantissa, int *carry_exp);

#endif
