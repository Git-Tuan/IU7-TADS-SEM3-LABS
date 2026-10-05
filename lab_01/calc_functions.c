#include "calc_functions.h"
#include "status.h"
#include <stdio.h>
#include <string.h>

return_code_t convert_numerical_string_to_int(const char *str, int *dest, size_t *dest_len)
{
    return_code_t rc = STATUS_OK;
    size_t len = strlen(str);
    int dest_idx = 0;

    for (int i = len - 1; i >= 0; i -= 4)
    {
        int start = (i - 3 < 0) ? 0 : i - 3;
        int val = 0;
        for (int j = start; j <= i; j++)
            val = val * 10 + (str[j] - '0');
        dest[dest_idx++] = val;
    }

    *dest_len = dest_idx;

    return rc;
}

void multiply_long_numbers(const long_number_t *a, const long_number_t *b, long_number_t *result)
{
    int tmp[MAX_RESULT_TETRADS] = {0};
    size_t max_len = a->len + b->len;
    const int base = 10000;

    for (size_t i = 0; i < a->len; ++i)
        for (size_t j = 0; j < b->len; ++j)
            tmp[i + j] += a->tetrads[i] * b->tetrads[j];

    for (size_t k = 0; k < max_len - 1; ++k)
    {
        int carry = tmp[k] / base;
        tmp[k] %= base;
        tmp[k + 1] += carry;
    }
    int carry = tmp[max_len - 1] / base;
    tmp[max_len - 1] %= base;
    if (carry > 0)
    {
        tmp[max_len] = carry;
        max_len++;
    }

    while (max_len > 1 && tmp[max_len - 1] == 0)
        max_len--;

    for (size_t i = 0; i < max_len; ++i)
        result->tetrads[i] = tmp[i];
    result->len = max_len;
    result->exp = a->exp + b->exp;
}

void tetrads_to_string(char *mant_str, const int *tetrads, size_t len)
{
    char *ptr = mant_str;
    for (size_t i = len; i > 0; --i)
    {
        int val = tetrads[i - 1];
        if (i == len)
            ptr += sprintf(ptr, "%d", val);
        else
            ptr += sprintf(ptr, "%04d", val);
    }
}

void round_mantissa(char *mantissa, int *carry_exp)
{
    printf("Here is %s\n", mantissa);
    int len = strlen(mantissa);
    *carry_exp = 0;

    if (len <= MAX_MANTISSA_LEN - 1)
        return;

    char round_digit = mantissa[MAX_MANTISSA_LEN - 1];
    mantissa[MAX_MANTISSA_LEN - 1] = '\0';

    if (round_digit >= '5')
    {
        int flag = 1;
        int i = MAX_MANTISSA_LEN - 2;
        while (i >= 0 && flag)
        {
            if (mantissa[i] == '9')
                mantissa[i--] = '0';
            else
            {
                mantissa[i] = mantissa[i] + 1;
                flag = 0;
            }
        }
        if (i < 0)
        {
            mantissa[0] = '1';
            for (int j = 1; j < MAX_MANTISSA_LEN - 1; j++)
                mantissa[j] = '0';
            *carry_exp = 1;
        }
    }
    else
        printf("HELLO?\n");
}
