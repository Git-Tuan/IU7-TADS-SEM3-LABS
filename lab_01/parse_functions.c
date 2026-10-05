#include "parse_functions.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static void reset_to_zero_if_zero(floating_point_t *float_number)
{
    int all_zero = 1;
    int j = 0;
    while (j < float_number->mantissa_len && all_zero == 1)
    {
        if (float_number->mantissa[j] != '0')
        {
            all_zero = 0;
        }
        j++;
    }
    if (all_zero == 1)
    {
        for (int i = 1; i < float_number->mantissa_len; i++)
            float_number->mantissa[i] = '\0';

        float_number->mantissa[0] = '0';
        float_number->mantissa_len = 1;
        float_number->exponent = 0;
    }
}

static return_code_t skip_delimiters(const char *float_string, size_t length, size_t *current_index)
{
    return_code_t rc = STATUS_OK;

    while (*current_index < length && (float_string[*current_index] == ' ' || float_string[*current_index] == '\t'))
        (*current_index)++;
    if (*current_index == length)
        rc = STATUS_FLOAT_INPUT_ERROR;
    return rc;
}

static return_code_t count_exponent(const char *float_string, size_t length, size_t *current_index, int *exponent)
{
    return_code_t rc = STATUS_OK;
    int exp_val = 0;
    int digit_count = 0;

    while (*current_index < length && isdigit(float_string[*current_index]))
    {
        exp_val = exp_val * 10 + (float_string[*current_index] - '0');
        (*current_index)++;
        digit_count++;
    }

    if (digit_count == 0 || (isdigit(float_string[*current_index]) == 0 && *current_index < length))
        rc = STATUS_EXPONENT_INPUT_ERROR;
    else if (exp_val > MAX_EXPONENT)
        rc = STATUS_EXPONENT_OVERFLOW_ERROR;
    else
        *exponent = exp_val;

    return rc;
}

static return_code_t parse_exponent(const char *float_string, size_t len, size_t *idx, int *exponent)
{
    return_code_t rc = STATUS_OK;
    int sign = 1;
    int val = 0;

    rc = skip_delimiters(float_string, len, idx);
    if (rc == STATUS_OK)
    {
        if (*idx < len && (float_string[*idx] == '+' || float_string[*idx] == '-'))
        {
            if (float_string[*idx] == '-')
                sign = -1;
            (*idx)++;
            rc = skip_delimiters(float_string, len, idx);
        }
        if (rc == STATUS_OK)
        {
            rc = count_exponent(float_string, len, idx, &val);
            if (rc == STATUS_OK)
                *exponent = sign * val;
        }
    }

    return rc;
}

return_code_t process_float_number(const char *float_string, size_t length, floating_point_t *float_number)
{
    return_code_t rc = STATUS_OK;
    const char *digits = "0123456789";
    int has_exponent = 0, has_delim = 0, index = 0;
    int point_pos = -1;

    if (*float_string == '-' || *float_string == '+')
    {
        if (*float_string == '-')
            float_number->sign = '-';
        index++;
    }
    else if (*float_string == '\0')
        rc = STATUS_FLOAT_INPUT_ERROR;

    size_t i = index;
    int started = 0;

    for (; rc == STATUS_OK && i < length && !has_exponent && !has_delim; i++)
    {
        if (strchr(digits, float_string[i]) != NULL)
        {
            if (started || float_string[i] != '0' || point_pos != -1)
            {
                if (float_number->mantissa_len < MAX_MANTISSA_LEN - 1)
                {
                    float_number->mantissa[float_number->mantissa_len++] = float_string[i];
                    started = 1;
                }
                else
                    rc = STATUS_FLOAT_OVERFLOW;
            }
            else if (float_string[i] == '0' && length == 1)
                float_number->mantissa[float_number->mantissa_len++] = float_string[i];
        }
        else if (float_string[i] == '.')
        {
            if (point_pos == -1)
            {
                point_pos = float_number->mantissa_len;
                started = 1;
            }
            else
                rc = STATUS_FLOAT_INPUT_ERROR;
        }
        else if (float_string[i] == 'E' || float_string[i] == 'e')
            has_exponent = 1;
        else if (float_string[i] == ' ' || float_string[i] == '\t')
            has_delim = 1;
        else
            rc = STATUS_FLOAT_INPUT_ERROR;
    }

    if (isdigit(float_number->mantissa[0]) == 0)
        rc = STATUS_FLOAT_INPUT_ERROR;

    if (rc == STATUS_OK)
        reset_to_zero_if_zero(float_number);

    if (has_delim && rc == STATUS_OK)
    {
        rc = skip_delimiters(float_string, length, &i);
        if (rc == STATUS_OK)
        {
            if (i < length && (float_string[i] == 'E' || float_string[i] == 'e'))
            {
                i++;
                rc = parse_exponent(float_string, length, &i, &float_number->exponent);
            }
            else
                rc = STATUS_MISS_EXPONENT;
        }
    }
    else if (has_exponent && rc == STATUS_OK)
        rc = parse_exponent(float_string, length, &i, &float_number->exponent);

    if (point_pos != -1)
    {
        int shift = float_number->mantissa_len - point_pos;
        float_number->exponent -= shift;
    }

    float_number->mantissa[MAX_MANTISSA_LEN - 1] = '\0';

    return rc;
}

return_code_t get_integer(integer_t *buffer)
{
    return_code_t rc = STATUS_OK;

    const char *digits = "0123456789";
    char current_digit;
    int index = 0, is_significant = 0;

    current_digit = getchar();

    if (current_digit == '-' || current_digit == '+')
    {
        if (current_digit == '-')
            buffer->sign = '-';
    }
    else if (current_digit == '\n' || current_digit == EOF)
        rc = STATUS_INTEGER_INPUT_ERROR;
    else if (strchr(digits, current_digit) == NULL)
        rc = STATUS_INTEGER_INPUT_ERROR;
    else if (current_digit != '0')
    {
        buffer->integer_str_format[index++] = current_digit;
        is_significant = 1;
    }

    while (rc == STATUS_OK && (current_digit = getchar()) != '\n' && current_digit != EOF)
    {
        if (strchr(digits, current_digit) != NULL)
        {
            if (current_digit != '0' || is_significant != 0)
            {
                if (index < MAX_INT_LENGTH - 1)
                {
                    buffer->integer_str_format[index] = current_digit;
                    index++;
                    is_significant = 1;
                }
                else
                    rc = STATUS_INTEGER_OVERFLOW;
            }
        }
        else
            rc = STATUS_INTEGER_INPUT_ERROR;
    }

    if (is_significant == 0 && rc == STATUS_OK)
    {
        buffer->integer_str_format[index++] = '0';
        buffer->exponent = 0;
    }

    buffer->integer_str_format[index] = '\0';

    if (rc == STATUS_OK)
        buffer->exponent = strlen(buffer->integer_str_format);

    return rc;
}
