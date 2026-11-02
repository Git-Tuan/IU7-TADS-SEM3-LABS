#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "app_context.h"
#include "errors.h"
#include "matrix_types.h"
#include "ui.h"
#include "utils.h"

static rc_t process_choice(AppContext *context, int choice, int *exit_flag);

static DenseMatrix s_dense_a;
static DenseMatrix s_dense_b;
static DenseMatrix s_dense_c;

int main(void)
{
    AppContext context;
    int choice = 0;
    int exit_flag = 0;
    rc_t status = STATUS_OK;

    context_init(&context);
    random_seed((unsigned int)time(NULL));

    printf("Программа обработки разреженных матриц (вариант 5).\n");
    printf("Индексы вводятся 0-базовые. Диапазон размеров: 1..%d.\n", MAX_DIM);

    while (!exit_flag)
    {
        show_menu();
        status = read_int("Ваш выбор: ", 0, 7, &choice);
        if (status == STATUS_OK)
        {
            status = process_choice(&context, choice, &exit_flag);
            if (status != STATUS_OK)
            {
                print_error_msg(status);
            }
        }
        else
        {
            print_error_msg(status);
        }
    }

    context_free(&context);
    printf("Программа завершена.\n");
    return status;
}


static rc_t process_choice(AppContext *context, int choice, int *exit_flag)
{
    rc_t status = STATUS_OK;
    *exit_flag = 0;
    if (choice == 1)
    {
        status = handle_manual_input(context);
    }
    else if (choice == 2)
    {
        status = handle_random_generation(context);
    }
    else if (choice == 3)
    {
        status = handle_show_matrices(context);
    }
    else if (choice == 4)
    {
        status = handle_sparse_multiply(context);
    }
    else if (choice == 5)
    {
        status = handle_dense_multiply(context, &s_dense_a, &s_dense_b, &s_dense_c);
    }
    else if (choice == 6)
    {
        status = handle_benchmark(context);
    }
    else if (choice == 7)
    {
        status = handle_free_all(context);
        if (status == STATUS_OK)
        {
            printf("Память освобождена.\n");
        }
    }
    else if (choice == 0)
    {
        *exit_flag = 1;
    }
    else
    {
        status = STATUS_MENU_CHOICE_ERR;
    }
    return status;
}