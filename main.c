#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_INT_LENGTH 42
#define MAX_MANTISSA_LEN 31

typedef enum
{
    STATUS_OK = 0,
    STATUS_INTEGER_INPUT_ERROR = 1,
    STATUS_INTEGER_OVERFLOW = 2,
    STATUS_FLOAT_INPUT_ERROR = 3,
    STATUS_FLOAT_OVERFLOW = 4,
    ERROR_SKIP_DELIMITERS = 5,
    STATUS_MISS_EXPONENT = 6,
    STATUS_MISS_SIGN = 7,
    STATUS_EXPONENT_INPUT_ERROR = 8,
    STATUS_EXPONENT_OVERFLOW_ERROR = 9
} return_code_t;

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
    char integer_string_format[MAX_INT_LENGTH];
} integer_t;


static void is_zero(floating_point_t *float_number)
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
    else if (exp_val > 99999) 
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
            if (float_string[*idx] == '-') sign = -1;
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

// \n
// +.
return_code_t get_float_number(const char *float_string, size_t length, floating_point_t *float_number)
{
    return_code_t rc = STATUS_OK;
    const char *digits = "0123456789";
    int has_exponent = 0, has_delim = 0, index = 0;
    int point_pos = -1;

    if (*float_string == '-')
    {
        float_number->sign = '-';
        index++;
    }
    else if (*float_string == '+')
    {
        float_number->sign = '+';
        index++;
    }
    else if (*float_string == '\n')
        rc = STATUS_FLOAT_INPUT_ERROR;

    size_t i = index;

    for (; rc == STATUS_OK && i < length && !has_exponent && !has_delim; i++)
    {
        if (strchr(digits, float_string[i]) != NULL)
        { 
            if (float_string[i] != '0' || point_pos != -1)
            {
                if (float_number->mantissa_len < MAX_MANTISSA_LEN - 1)
                {
                    float_number->mantissa[float_number->mantissa_len] = float_string[i];
                    float_number->mantissa_len++;
                }
                else
                    rc = STATUS_FLOAT_OVERFLOW;
            }
        }    
        else if (float_string[i] == '.')
        {
            if (point_pos == -1)
                point_pos = float_number->mantissa_len;
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

    if (rc == STATUS_OK)
        is_zero(float_number);

    if (has_delim && rc == STATUS_OK) 
    {
        rc = skip_delimiters(float_string, length, &i);
        if (rc == STATUS_OK) {
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

    float_number->mantissa[MAX_MANTISSA_LEN-1] = '\0';

    return rc;
}

return_code_t get_integer(char *int_string_format)
{
    return_code_t rc = STATUS_OK;

    const char *digits = "0123456789";
    char current_digit;
    int index = 0, is_significant = 0, has_sign = 0;

    current_digit = getchar();

    if (current_digit == '-' || current_digit == '+')
    {
        has_sign = 1;
        int_string_format[index++] = current_digit;
    }
    else if (current_digit == '\n' || current_digit == EOF)
    {
        rc = STATUS_INTEGER_INPUT_ERROR;
        int_string_format[index] = '\0';
    }
    else if (strchr(digits, current_digit) == NULL)
    {
        rc = STATUS_INTEGER_INPUT_ERROR;
        int_string_format[index] = '\0';
    }
    else if (current_digit != '0')
    {
        int_string_format[index++] = current_digit;
        is_significant = 1;
    }

    while ( rc == STATUS_OK && (current_digit = getchar()) != '\n' && current_digit != EOF)
    {
        if (strchr(digits, current_digit) != NULL)
        {
            if (current_digit != '0' || is_significant != 0)
            {
                if (index < MAX_INT_LENGTH - 1)
                {
                    int_string_format[index] = current_digit;
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
        int_string_format[index++] = '0';

    if (has_sign == 0 && index == 41)
        rc = STATUS_INTEGER_OVERFLOW;

    int_string_format[index] = '\0';

    return rc;

}


return_code_t convert_numerical_string_to_int(const char *str, int *dest, size_t *dest_len)
{
    return_code_t rc = STATUS_OK;
    size_t len = strlen(str);
    int dest_idx = 0;

    for (int i = len - 1; i >= 0; i -= 4)
    {
        int val = 0;
        int start = (i - 3 < 0) ? 0 : i - 3;
        
        for (int j = i; j >= start; j--)
        {
            val = val * 10 + (str[j] - '0');
        }
        
        dest[dest_idx++] = val;
    }

    *dest_len = dest_idx;

    return rc;
}

void print_array(int *arr, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(void)
{
    return_code_t rc = STATUS_OK;

    char int_string_format[MAX_INT_LENGTH];
    int integer_arr[10] = {0};
    size_t integer_arr_len = 0;
    int float_arr[8] = {0};
    size_t float_arr_len = 0;
    char float_string_format[64];
    floating_point_t floating_pt = { .sign = 'N', .mantissa_len = 0 };
    

    printf("Enter integer (max number of digits - 40)\n"
        "Follow this format: +-d1d2d3d4...\n"
        "Awaiting for your response: ");
    
    rc = get_integer(int_string_format);
    
    if (rc == STATUS_OK)   
    {
        printf("Integer: %s\n", int_string_format);
        printf("-------------------------------------------------------------------------\n");
    }
    else
    {
        printf("Error occured\n");
        printf("-------------------------------------------------------------------------\n");
    }
    
    if (rc == STATUS_OK)
    {
        
        printf("Enter float point number (max length of mantissa - 30; exponent - 5)\n"
            "Follow this format: +-d1.d2d3d4 OR +-d1.d2d3d4+-E d1d2...\n"
            "Awaiting for your response: ");

        fgets(float_string_format, 64, stdin);
        float_string_format[strcspn(float_string_format, "\n")] = '\0';
        size_t length = strlen(float_string_format);
        rc = get_float_number(float_string_format, length, &floating_pt);

        if (rc == STATUS_OK)
        {
            convert_numerical_string_to_int(int_string_format, integer_arr, &integer_arr_len);
            convert_numerical_string_to_int(floating_pt.mantissa, float_arr, &float_arr_len);
            print_array(integer_arr, integer_arr_len);
            print_array(float_arr, float_arr_len);
        }
        
        printf("Входные данные: %s\n", float_string_format);
        if (rc == STATUS_OK)
        {
            printf("Мантисса: %s\n", floating_pt.mantissa);
            printf("Знак: %c\n", floating_pt.sign);
            printf("Длина мантиссы: %d\n", floating_pt.mantissa_len);
            printf("Степень: %d\n", floating_pt.exponent);
            printf("-------------------------------------------------------------------------\n");

        }
        else
        {
            printf("Error occured: err_code - %d\n", rc);
            printf("-------------------------------------------------------------------------\n");
        }
        
       
    }
    
    return rc;
}
