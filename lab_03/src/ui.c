#include "ui.h"
#include "benchmark.h"
#include "csc.h"
#include "csr.h"
#include "dense.h"
#include "io.h"
#include "matrix_types.h"
#include "sparse_ops.h"
#include "utils.h"
#include <stdio.h>

rc_t handle_manual_input(AppContext *context)
{
    rc_t status = STATUS_OK;
    printf("\nРучной ввод матриц (координатный способ)\n");
    printf("Матрица A (CSR):\n");
    status = input_coordinate_csr(&context->csr_a);
    if (status == STATUS_OK)
    {
        printf("Матрица B (CSC):\n");
        status = input_coordinate_csc(&context->csc_b);
        if (status == STATUS_OK)
        {
            context->has_sparse_data = 1;
            context->has_dense_data = 0;
            printf("Ввод выполнен.\n");
        }
    }
    return status;
}

rc_t handle_random_generation(AppContext *context)
{
    rc_t status = STATUS_OK;
    int rows = 0;
    int inner = 0;
    int cols = 0;
    double density = 0.0;

    printf("\nАвтоматическая генерация разреженных матриц:\n");
    status = read_int("Число строк A: ", 1, MAX_DIM, &rows);
    if (status == STATUS_OK)
    {
        status = read_int("Число столбцов A (= строк B): ", 1, MAX_DIM, &inner);
    }
    if (status == STATUS_OK)
    {
        status = read_int("Число столбцов B: ", 1, MAX_DIM, &cols);
    }
    if (status == STATUS_OK)
    {
        status = read_double("Плотность (0.0 .. 1.0): ", 0.0, 1.0, &density);
    }
    if (status == STATUS_OK)
    {
        status = generate_random_csr(&context->csr_a, rows, inner, density);
    }
    if (status == STATUS_OK)
    {
        status = generate_random_csc(&context->csc_b, inner, cols, density);
    }
    if (status == STATUS_OK)
    {
        context->has_sparse_data = 1;
        context->has_dense_data = 0;
        printf("Сгенерировано: nnz(A)=%d, nnz(B)=%d.\n", context->csr_a.nnz, context->csc_b.nnz);
    }
    return status;
}

rc_t handle_show_matrices(AppContext *context)
{
    rc_t status = STATUS_OK;
    printf("\nПросмотр матриц\n");
    if (!context->has_sparse_data)
    {
        status = STATUS_MATRIX_NOT_SET;
    }
    else
    {
        printf("\nМатрица A (CSR)\n");
        printf("Размер %dx%d, nnz=%d\n", context->csr_a.rows, context->csr_a.cols, context->csr_a.nnz);
        csr_print_arrays(&context->csr_a);
        printf("Плотный вид:\n");
        csr_print_dense(&context->csr_a);

        printf("\nМатрица B (CSC)\n");
        printf("Размер %dx%d, nnz=%d\n", context->csc_b.rows, context->csc_b.cols, context->csc_b.nnz);
        csc_print_arrays(&context->csc_b);
        printf("Плотный вид:\n");
        csc_print_dense(&context->csc_b);

        if (context->csr_c.rows > 0)
        {
            printf("\nМатрица C = A*B (CSR)\n");
            printf("Размер %dx%d, nnz=%d\n", context->csr_c.rows, context->csr_c.cols, context->csr_c.nnz);
            csr_print_arrays(&context->csr_c);
            printf("Плотный вид:\n");
            csr_print_dense(&context->csr_c);
        }
    }
    return status;
}

rc_t handle_sparse_multiply(AppContext *context)
{
    rc_t status = STATUS_OK;
    printf("\nУмножение разреженных матриц (A_CSR * B_CSC = C_CSR)\n");
    if (!context->has_sparse_data)
    {
        status = STATUS_MATRIX_NOT_SET;
    }
    else
    {
        csr_free(&context->csr_c);
        double t0 = get_time_seconds();
        status = csr_csc_multiply(&context->csr_a, &context->csc_b, &context->csr_c);
        double t1 = get_time_seconds();
        if (status == STATUS_OK)
        {
            printf("Готово. nnz(C)=%d, время %.6f с.\n", context->csr_c.nnz, t1 - t0);
            if (context->csr_c.nnz == 0)
            {
                printf("Внимание: результат полностью нулевой.\n");
            }
            printf("Память (A+B+C): %zu байт.\n", csr_memory_bytes(&context->csr_a) +
                                                      csc_memory_bytes(&context->csc_b) +
                                                      csr_memory_bytes(&context->csr_c));
        }
    }
    return status;
}

rc_t handle_dense_multiply(AppContext *context, DenseMatrix *dense_a, DenseMatrix *dense_b, DenseMatrix *dense_c)
{
    rc_t status = STATUS_OK;
    printf("\n=== Умножение плотных матриц ===\n");
    if (!context->has_sparse_data)
    {
        status = STATUS_MATRIX_NOT_SET;
    }
    else
    {
        status = csr_to_dense(&context->csr_a, dense_a);
        if (status == STATUS_OK)
        {
            status = csc_to_dense(&context->csc_b, dense_b);
        }
        if (status == STATUS_OK)
        {
            double t0 = get_time_seconds();
            status = dense_multiply(dense_a, dense_b, dense_c);
            double t1 = get_time_seconds();
            if (status == STATUS_OK)
            {
                context->has_dense_data = 1;
                printf("Готово. Время %.6f с.\n", t1 - t0);
                printf("Память (A+B+C): %zu байт.\n",
                       dense_memory_bytes(dense_a) + dense_memory_bytes(dense_b) + dense_memory_bytes(dense_c));
            }
        }
    }
    return status;
}

rc_t handle_benchmark(AppContext *context)
{
    rc_t status = STATUS_OK;
    int rows = 0;
    int inner = 0;
    int cols = 0;
    double density_from = 0.0;
    double density_to = 0.0;
    double density_step = 0.0;
    int choice = 0;
    int scanned = 0;

    printf("\nСравнение эффективности\n");
    printf("1) Сравнить уже введённые матрицы\n");
    printf("2) Запустить серию тестов по плотностям\n");
    printf("Выбор: ");
    scanned = scanf("%d", &choice);
    if (scanned != 1)
    {
        flush_input();
        status = STATUS_STRING_INPUT_ERROR;
    }
    else if (choice == 1)
    {
        if (context->has_sparse_data)
        {
            status = compare_single_run(&context->csr_a, &context->csc_b);
        }
        else
        {
            status = STATUS_MATRIX_NOT_SET;
        }
    }
    else if (choice == 2)
    {
        status = read_int("Строк A: ", 1, 500, &rows);
        if (status == STATUS_OK)
        {
            status = read_int("Общая размерность (cols A = rows B):  ", 1, 500, &inner);
        }
        if (status == STATUS_OK)
        {
            status = read_int("Столбцов B: ", 1, 500, &cols);
        }
        if (status == STATUS_OK)
        {
            status = read_double("Плотность от: ", 0.0, 1.0, &density_from);
        }
        if (status == STATUS_OK)
        {
            status = read_double("Плотность до: ", 0.0, 1.0, &density_to);
        }
        if (status == STATUS_OK)
        {
            status = read_double("Шаг: ", 0.001, 1.0, &density_step);
        }
        if (status == STATUS_OK)
        {
            status = run_benchmark_suite(rows, inner, cols, density_from, density_to, density_step);
        }
    }
    else
    {
        status = STATUS_MENU_CHOICE_ERR;
    }
    return status;
}

rc_t handle_free_all(AppContext *context)
{
    context_free(context);
    return STATUS_OK;
}
