#include "csc.h"
#include <stdio.h>
#include <stdlib.h>

static void csc_build_col_pointers_from_counts(CscMatrix *matrix, const int *counts, int cols)
{
    int acc = 0;
    int i = 0;
    while (i < cols)
    {
        matrix->col_pointers[i] = acc;
        acc = acc + counts[i];
        i = i + 1;
    }
    matrix->col_pointers[cols] = acc;
}

static rc_t csc_place_triplets_into_matrix(CscMatrix *matrix, int cols, int count, const int *trows, const int *tcols,
                                           const double *tvalues)
{
    rc_t status = STATUS_OK;
    int *cursor = (int *)malloc(cols * sizeof(int));
    if (cursor == NULL)
    {
        status = STATUS_MEMORY_ALLOC_ERROR;
    }
    else
    {
        int i = 0;
        while (i < cols)
        {
            cursor[i] = matrix->col_pointers[i];
            i = i + 1;
        }
        i = 0;
        while (i < count)
        {
            int c = tcols[i];
            int p = cursor[c];
            matrix->values[p] = tvalues[i];
            matrix->row_indices[p] = trows[i];
            cursor[c] = p + 1;
            i = i + 1;
        }
        free(cursor);
    }
    return status;
}

static void csc_print_dense_data(const DenseMatrix *matrix)
{
    int i = 0;
    while (i < matrix->rows)
    {
        int j = 0;
        while (j < matrix->cols)
        {
            printf("%8.3f ", matrix->data[i][j]);
            j = j + 1;
        }
        printf("\n");
        i = i + 1;
    }
}

/* ---------- публичный интерфейс ---------- */

rc_t csc_init(CscMatrix *matrix, int rows, int cols, int nnz)
{
    rc_t status = STATUS_OK;
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->nnz = nnz;
    matrix->values = NULL;
    matrix->row_indices = NULL;
    matrix->col_pointers = NULL;

    if (rows <= 0 || cols <= 0 || nnz < 0)
    {
        status = STATUS_INVALID_DIMENSION_ERROR;
    }
    else
    {
        matrix->values = (double *)malloc((nnz > 0 ? nnz : 1) * sizeof(double));
        matrix->row_indices = (int *)malloc((nnz > 0 ? nnz : 1) * sizeof(int));
        matrix->col_pointers = (int *)malloc((cols + 1) * sizeof(int));
        if (matrix->values == NULL || matrix->row_indices == NULL || matrix->col_pointers == NULL)
        {
            csc_free(matrix);
            status = STATUS_MEMORY_ALLOC_ERROR;
        }
        else
        {
            int k = 0;
            while (k <= cols)
            {
                matrix->col_pointers[k] = 0;
                k = k + 1;
            }
        }
    }
    return status;
}

void csc_free(CscMatrix *matrix)
{
    if (matrix->values != NULL)
    {
        free(matrix->values);
        matrix->values = NULL;
    }
    if (matrix->row_indices != NULL)
    {
        free(matrix->row_indices);
        matrix->row_indices = NULL;
    }
    if (matrix->col_pointers != NULL)
    {
        free(matrix->col_pointers);
        matrix->col_pointers = NULL;
    }
    matrix->rows = 0;
    matrix->cols = 0;
    matrix->nnz = 0;
}

size_t csc_memory_bytes(const CscMatrix *matrix)
{
    return (size_t)matrix->nnz * sizeof(double) + (size_t)matrix->nnz * sizeof(int) +
           (size_t)(matrix->cols + 1) * sizeof(int);
}

rc_t csc_to_dense(const CscMatrix *src, DenseMatrix *dst)
{
    rc_t status = STATUS_OK;
    if (src->rows > MAX_DIM || src->cols > MAX_DIM)
    {
        status = STATUS_MATRIX_TOO_LARGE;
    }
    else
    {
        int i = 0;
        int j = 0;
        dst->rows = src->rows;
        dst->cols = src->cols;
        i = 0;
        while (i < src->rows)
        {
            j = 0;
            while (j < src->cols)
            {
                dst->data[i][j] = 0.0;
                j = j + 1;
            }
            i = i + 1;
        }
        j = 0;
        while (j < src->cols)
        {
            int k = src->col_pointers[j];
            int end = src->col_pointers[j + 1];
            while (k < end)
            {
                dst->data[src->row_indices[k]][j] = src->values[k];
                k = k + 1;
            }
            j = j + 1;
        }
    }
    return status;
}

rc_t csc_from_triplets(CscMatrix *matrix, int rows, int cols, int count, const int *trows, const int *tcols,
                       const double *tvalues)
{
    rc_t status = STATUS_OK;
    int *counts = (int *)calloc(cols, sizeof(int));
    if (counts == NULL)
    {
        status = STATUS_MEMORY_ALLOC_ERROR;
    }
    else
    {
        int i = 0;
        while (i < count)
        {
            counts[tcols[i]] = counts[tcols[i]] + 1;
            i = i + 1;
        }
        status = csc_init(matrix, rows, cols, count);
        if (status == STATUS_OK)
        {
            csc_build_col_pointers_from_counts(matrix, counts, cols);
            status = csc_place_triplets_into_matrix(matrix, cols, count, trows, tcols, tvalues);
            if (status != STATUS_OK)
            {
                csc_free(matrix);
            }
        }
        free(counts);
    }
    return status;
}

void csc_print_arrays(const CscMatrix *matrix)
{
    int k = 0;
    printf("  JA (col_pointers), %d значений: ", matrix->cols + 1);
    k = 0;
    while (k <= matrix->cols)
    {
        printf("%d ", matrix->col_pointers[k]);
        k = k + 1;
    }
    printf("\n");
    printf("  IA (row_indices),  %d значений: ", matrix->nnz);
    k = 0;
    while (k < matrix->nnz)
    {
        printf("%d ", matrix->row_indices[k]);
        k = k + 1;
    }
    printf("\n");
    printf("  A  (values),       %d значений: ", matrix->nnz);
    k = 0;
    while (k < matrix->nnz)
    {
        printf("%.4g ", matrix->values[k]);
        k = k + 1;
    }
    printf("\n");
}

void csc_print_dense(const CscMatrix *matrix)
{
    if (matrix->rows > 30 || matrix->cols > 30)
    {
        printf("  (матрица слишком большая для вывода в плотном виде)\n");
    }
    else
    {
        DenseMatrix *tmp = (DenseMatrix *)malloc(sizeof(DenseMatrix));
        if (tmp != NULL)
        {
            if (csc_to_dense(matrix, tmp) == STATUS_OK)
            {
                csc_print_dense_data(tmp);
            }
            free(tmp);
        }
    }
}
