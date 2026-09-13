#include <stdio.h>
#include <string.h>
#include "parse_functions.h"
#include "status.h"
#include "defines.h"
#include "calc_functions.h"
#include "numbers_io.h"
#include "errors.h"

int main(void)
{
    return_code_t rc = STATUS_OK;

    integer_t integer = { .sign = '+' };
    floating_point_t floating_pt = { .sign = '+', .mantissa_len = 0 };
    long_number_t result = { .len = 0, .exp = 0 };

    rc = read_integer(&integer);
    
    if (rc == STATUS_OK)
    {
        rc = read_float(&floating_pt);
        if (rc == STATUS_OK)
        {
            multiply_integer_by_float(&integer, &floating_pt, &result);
            print_result_from(&integer, &floating_pt, &result);
        }
    }
    if (rc != STATUS_OK)
        print_error_msg(rc);
    
    return rc;
}

