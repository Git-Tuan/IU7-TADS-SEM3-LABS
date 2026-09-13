#include <stdio.h>
#include <string.h>
#include "parse_functions.h"
#include "status.h"
#include "defines.h"
#include "calc_functions.h"

int main(void)
{
    return_code_t rc = STATUS_OK;

    integer_t integer = { .sign = '+' };
    floating_point_t floating_pt = { .sign = '+', .mantissa_len = 0 };
    char float_string_format[64];
    long_number_t num_a = { .len = 0, .exp = 0 };
    long_number_t num_b = { .len = 0, .exp = 0 };
    long_number_t result = { .len = 0, .exp = 0 };

    printf("Enter integer (max number of digits - 40)\n"
        "Follow this format: +-d1d2d3d4...\n"
        "Waiting for your response: ");
    
    rc = get_integer(&integer);
    
    if (rc == STATUS_OK)   
    {
        printf("Integer: %s\n", integer.integer_str_format);
        printf("Integer sign: %c\n", integer.sign);
    }
    
    if (rc == STATUS_OK)
    {
        
        printf("Enter float point number (max length of mantissa - 30; exponent - 5)\n"
            "Follow this format: +-d1.d2d3d4 OR +-d1.d2d3d4+-E d1d2...\n"
            "Waiting for your response: ");

        fgets(float_string_format, 64, stdin);
        float_string_format[strcspn(float_string_format, "\n")] = '\0';
        size_t length = strlen(float_string_format);
        rc = process_float_number(float_string_format, length, &floating_pt);
        
        if (rc == STATUS_OK)
        {
            printf("Mantissa: %s\n", floating_pt.mantissa);
            printf("Sign: %c\n", floating_pt.sign);
            printf("Mantissa's length: %d\n", floating_pt.mantissa_len);
            printf("Power: %d\n", floating_pt.exponent);

        }
        else
            printf("Error occured: err_code - %d\n", rc);

        if (rc == STATUS_OK)
        {
            convert_numerical_string_to_int(integer.integer_str_format, num_a.tetrads, &num_a.len);
            convert_numerical_string_to_int(floating_pt.mantissa, num_b.tetrads, &num_b.len);
            num_a.exp = 0;
            num_b.exp = floating_pt.exponent;

            multiply_long_numbers(&num_a, &num_b, &result);

            char mant_str[256];
            char result_sign = (integer.sign != floating_pt.sign) ? '-' : '+';
            tetrads_to_string(mant_str, result.tetrads, result.len);
            int full_len = strlen(mant_str);
            int normalized_exp = result.exp + full_len;

            int carry_exp = 0;
            round_mantissa(mant_str, &carry_exp);
            normalized_exp += carry_exp;

            printf("%d\n", normalized_exp < 99999);
            if (strcmp(mant_str, "0") == 0)
                printf("Result: 0\n");
            else if (normalized_exp < 0 && normalized_exp < -99999)
                printf("Error: machine precision error\n");
            else if (normalized_exp > 0 && normalized_exp > 99999)
                printf("Error: overflow\n");
            else
                printf("Result: %c0.%s E %+d\n", result_sign, mant_str, normalized_exp);
        }
        
       
    }
    
    return rc;
}
