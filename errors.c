#include <stdio.h>
#include "errors.h"

void print_error_msg(return_code_t rc)
{
    if (rc == STATUS_INTEGER_INPUT_ERROR)
        printf("Error: invalid symbols found; make sure you only use integers: 0-9\n");
    else if (rc == STATUS_INTEGER_OVERFLOW)
        printf("Error: invalid length of the integer; you can only type integers of length 40 at most\n");
    else if (rc == STATUS_FLOAT_INPUT_ERROR)
        printf("Error: invalid symbols found; follow this format: +-digits.digits E +- digits(signs are optional)\n");
    else if (rc == STATUS_FLOAT_OVERFLOW)
        printf("Error: invalid length of the mantissa / exponent; max mantissa length - 30; max exponent length - 5\n");
    else if (rc == ERROR_SKIP_DELIMITERS)
        printf("Error: invalid blank symbols found; make sure you do not have extra spaces before and after the number\n");
    else if (rc == STATUS_MISS_EXPONENT)
        printf("Error: invalid input of the exponent; missed exponent after: +-digits.digits E +- >>put number there, after sign<<\n");
    else if (rc == STATUS_MISS_SIGN)
        printf("Error: you forgot to put sign where it is necessary\n");
    else if (rc == STATUS_EXPONENT_INPUT_ERROR) 
        printf("Error: invalid symbols found in the exponent; make sure you only use integers: 0-9\n");
    else if (rc == STATUS_EXPONENT_OVERFLOW_ERROR)
        printf("Error: invalid length of the exponent; max lenth - 5\n");
}
