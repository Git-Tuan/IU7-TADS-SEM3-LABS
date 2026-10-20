#include "csr.h"
#include <stdio.h>
#include <stdlib.h>

static void build_row_pointers_from_counts(CsrMatrix *matrix, const int *counts, int rows)
{
    int acc = 0;
    int i = 0;
    while (i < rows)
    {
        matrix->row_pointers[i] = acc;
        acc = acc + counts[i];
        i = i + 1;
    }
    matrix->row_pointers[rows] = acc;
}

static rc_t place_triplets_into_csr(CsrMatrix *matrix, int rows, int count, const int *trows, const int *tcols,
                                    const double *tvalues)
{
    rc_t status = STATUS_OK;
    int *cursor = (int *)malloc(rows * sizeof(int));
    if (cursor == NULL)
    {
        status = STATUS_MEMORY_ALLOC_ERROR;
    }
    else
    {
        int i = 0;
        while (i < rows)
        {
            cursor[i] = matrix->row_pointers[i];
            i = i + 1;
        }
        i = 0;
        while (i < count)
        {
            int r = trows[i];
            int p = cursor[r];
            matrix->values[p] = tvalues[i];
            matrix->col_indices[p] = tcols[i];
            cursor[r] = p + 1;
            i = i + 1;
        }
        free(cursor);
    }
    return status;
}

static void print_dense_data(const DenseMatrix *matrix)
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

rc_t csr_init(CsrMatrix *matrix, int rows, int cols, int nnz)
{
    rc_t status = STATUS_OK;
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->nnz = nnz;
    matrix->values = NULL;
    matrix->col_indices = NULL;
    matrix->row_pointers = NULL;

    if (rows <= 0 || cols <= 0 || nnz < 0)
    {
        status = STATUS_INVALID_DIMENSION_ERROR;
    }
    else
    {
        matrix->values = (double *)malloc((nnz > 0 ? nnz : 1) * sizeof(double));
        matrix->col_indices = (int *)malloc((nnz > 0 ? nnz : 1) * sizeof(int));
        matrix->row_pointers = (int *)malloc((rows + 1) * sizeof(int));
        if (matrix->values == NULL || matrix->col_indices == NULL || matrix->row_pointers == NULL)
        {
            csr_free(matrix);
            status = STATUS_MEMORY_ALLOC_ERROR;
        }
        else
        {
            int k = 0;
            while (k <= rows)
            {
                matrix->row_pointers[k] = 0;
                k = k + 1;
            }
        }
    }
    return status;
}

void csr_free(CsrMatrix *matrix)
{
    if (matrix->values != NULL)
    {
        free(matrix->values);
        matrix->values = NULL;
    }
    if (matrix->col_indices != NULL)
    {
        free(matrix->col_indices);
        matrix->col_indices = NULL;
    }
    if (matrix->row_pointers != NULL)
    {
        free(matrix->row_pointers);
        matrix->row_pointers = NULL;
    }
    matrix->rows = 0;
    matrix->cols = 0;
    matrix->nnz = 0;
}

size_t csr_memory_bytes(const CsrMatrix *matrix)
{
    return (size_t)matrix->nnz * sizeof(double) + (size_t)matrix->nnz * sizeof(int) +
           (size_t)(matrix->rows + 1) * sizeof(int);
}

rc_t csr_to_dense(const CsrMatrix *src, DenseMatrix *dst)
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
        i = 0;
        while (i < src->rows)
        {
            int k = src->row_pointers[i];
            int end = src->row_pointers[i + 1];
            while (k < end)
            {
                dst->data[i][src->col_indices[k]] = src->values[k];
                k = k + 1;
            }
            i = i + 1;
        }
    }
    return status;
}

rc_t csr_from_triplets(CsrMatrix *matrix, int rows, int cols, int count, const int *trows, const int *tcols,
                       const double *tvalues)
{
    rc_t status = STATUS_OK;
    int *counts = (int *)calloc(rows, sizeof(int));
    if (counts == NULL)
    {
        status = STATUS_MEMORY_ALLOC_ERROR;
    }
    else
    {
        int i = 0;
        while (i < count)
        {
            counts[trows[i]] = counts[trows[i]] + 1;
            i = i + 1;
        }
        status = csr_init(matrix, rows, cols, count);
        if (status == STATUS_OK)
        {
            build_row_pointers_from_counts(matrix, counts, rows);
            status = place_triplets_into_csr(matrix, rows, count, trows, tcols, tvalues);
            if (status != STATUS_OK)
            {
                csr_free(matrix);
            }
        }
        free(counts);
    }
    return status;
}

void csr_print_arrays(const CsrMatrix *matrix)
{
    int k = 0;
    printf("  IA (row_pointers), %d значений: ", matrix->rows + 1);
    k = 0;
    while (k <= matrix->rows)
    {
        printf("%d ", matrix->row_pointers[k]);
        k = k + 1;
    }
    printf("\n");
    printf("  JA (col_indices),  %d значений: ", matrix->nnz);
    k = 0;
    while (k < matrix->nnz)
    {
        printf("%d ", matrix->col_indices[k]);
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

void csr_print_dense(const CsrMatrix *matrix)
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
            if (csr_to_dense(matrix, tmp) == STATUS_OK)
            {
                print_dense_data(tmp);
            }
            free(tmp);
        }
    }
}
