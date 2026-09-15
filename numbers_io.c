#include "numbers_io.h"
#include <stdio.h>
#include <string.h>

static void trim_trailing_zeros(char *mantissa)
{
    size_t len = strlen(mantissa);
    while (len > 1 && mantissa[len - 1] == '0')
    {
        len--;
    }
    mantissa[len] = '\0';
}

static void print_ruler(int max_len)
{
    printf(">");
    for (int i = 0; i < max_len; i++)
    {
        if ((i + 1) % 10 == 0)
            printf("%d", (i + 1) / 10);
        else
            printf("-");
    }
    printf("\n");
}

static return_code_t print_result(const char result_sign, char *mant_str, int normalized_exp)
{
    return_code_t rc = STATUS_OK;

    if (strcmp(mant_str, "0") == 0)
        printf("Result: 0\n");
    else if (normalized_exp < 0 && normalized_exp < -MAX_EXPONENT)
    {
        printf("Error: machine precision error\n");
        rc = STATUS_PRECISION_ERROR;
    }
    else if (normalized_exp > 0 && normalized_exp > MAX_EXPONENT)
    {
        printf("Error: overflow\n");
        rc = STATUS_MULTIPLICATION_OVERFLOW;
    }
    else
    {
        trim_trailing_zeros(mant_str);
        printf("Result: %c0.%s E %+d\n", result_sign, mant_str, normalized_exp);
    }
    return rc;
}

void multiply_integer_by_float(const integer_t *a, const floating_point_t *b, long_number_t *result)
{
    long_number_t num_a = {.len = 0, .exp = 0};
    long_number_t num_b = {.len = 0, .exp = 0};

    convert_numerical_string_to_int(a->integer_str_format, num_a.tetrads, &num_a.len);
    convert_numerical_string_to_int(b->mantissa, num_b.tetrads, &num_b.len);
    num_a.exp = 0;
    num_b.exp = b->exponent;

    multiply_long_numbers(&num_a, &num_b, result);
}

return_code_t print_result_from(const integer_t *a, const floating_point_t *b, const long_number_t *result)
{
    char mant_str[256];
    char result_sign = (a->sign != b->sign) ? '-' : '+';

    tetrads_to_string(mant_str, result->tetrads, result->len);

    int full_len = (int)strlen(mant_str);
    int normalized_exp = result->exp + full_len;

    int carry_exp = 0;
    round_mantissa(mant_str, &carry_exp);
    normalized_exp += carry_exp;

    return print_result(result_sign, mant_str, normalized_exp);
}

return_code_t read_float(floating_point_t *out)
{
    return_code_t rc = STATUS_OK;
    char buf[64];

    printf("Enter float point number (max length of mantissa - 30; exponent - 5)\n"
           "Follow this format: +-d1.d2d3d4 OR +-d1.d2d3d4+-E d1d2...\n"
           "Waiting for your response:\n");

    print_ruler(MAX_INT_LENGTH - 1);

    if (!fgets(buf, sizeof buf, stdin))
        rc = STATUS_FLOAT_INPUT_ERROR;
    else
        buf[strcspn(buf, "\n")] = '\0';
    if (rc == STATUS_OK)
        rc = process_float_number(buf, strlen(buf), out);
    if (rc == STATUS_OK)
    {
        printf("Mantissa: %s\n", out->mantissa);
        printf("Sign: %c\n", out->sign);
        printf("Mantissa's length: %d\n", out->mantissa_len);
        printf("Power: %d\n", out->exponent);
    }
    return rc;
}

return_code_t read_integer(integer_t *out)
{
    printf("Enter integer (max number of digits - 40)\n"
           "Follow this format: +-d1d2d3d4...\n"
           "Waiting for your response:\n");

    print_ruler(MAX_INT_LENGTH - 1);

    return_code_t rc = get_integer(out);
    if (rc == STATUS_OK)
    {
        printf("Integer: %s\n", out->integer_str_format);
        printf("Integer sign: %c\n", out->sign);
    }
    return rc;
}
