#ifndef PARSE_FUNCTIONS_H
#define PARSE_FUNCTIONS_H
#include "defines.h"
#include "status.h"
#include "stddef.h"

typedef struct
{
    char sign;
    char mantissa[MAX_MANTISSA_LEN];
    int mantissa_len;
    int exponent;
} floating_point_t;

typedef struct
{
    char sign;
    char integer_str_format[MAX_INT_LENGTH];
    int exponent;
} integer_t;

return_code_t get_integer(integer_t *buffer);
return_code_t process_float_number(const char *float_string, size_t length, floating_point_t *float_number);

#endif
