#include "sparse_ops.h"
#include "csr.h"
#include <stdlib.h>

static void load_row_into_marker(const CsrMatrix *left, int row, double *row_values, int *row_marker, int marker_id)
{
    int apos = left->row_pointers[row];
    int aend = left->row_pointers[row + 1];
    while (apos < aend)
    {
        int k = left->col_indices[apos];
        row_values[k] = left->values[apos];
        row_marker[k] = marker_id;
        apos = apos + 1;
    }
}

static double compute_cell_value(const CscMatrix *right, int col, const double *row_values, const int *row_marker,
                                 int marker_id)
{
    double sum = 0.0;
    int bpos = right->col_pointers[col];
    int bend = right->col_pointers[col + 1];
    while (bpos < bend)
    {
        int k = right->row_indices[bpos];
        if (row_marker[k] == marker_id)
        {
            sum = sum + row_values[k] * right->values[bpos];
        }
        bpos = bpos + 1;
    }
    return sum;
}

static int count_result_nnz(const CsrMatrix *left, const CscMatrix *right, double *row_values, int *row_marker,
                            int *marker_id_ptr)
{
    int result_nnz = 0;
    int i = 0;
    int m = left->rows;
    int p = right->cols;
    while (i < m)
    {
        int j = 0;
        *marker_id_ptr = *marker_id_ptr + 1;
        load_row_into_marker(left, i, row_values, row_marker, *marker_id_ptr);
        while (j < p)
        {
            double sum = compute_cell_value(right, j, row_values, row_marker, *marker_id_ptr);
            if (sum != 0.0)
            {
                result_nnz = result_nnz + 1;
            }
            j = j + 1;
        }
        i = i + 1;
    }
    return result_nnz;
}

static void fill_result_row(const CscMatrix *right, CsrMatrix *result, int *write_pos, const double *row_values,
                            const int *row_marker, int marker_id)
{
    int j = 0;
    int p = right->cols;
    while (j < p)
    {
        double sum = compute_cell_value(right, j, row_values, row_marker, marker_id);
        if (sum != 0.0)
        {
            result->values[*write_pos] = sum;
            result->col_indices[*write_pos] = j;
            *write_pos = *write_pos + 1;
        }
        j = j + 1;
    }
}

static void fill_result_matrix(const CsrMatrix *left, const CscMatrix *right, CsrMatrix *result, double *row_values,
                               int *row_marker, int *marker_id_ptr)
{
    int i = 0;
    int m = left->rows;
    int write_pos = 0;
    result->row_pointers[0] = 0;
    while (i < m)
    {
        *marker_id_ptr = *marker_id_ptr + 1;
        load_row_into_marker(left, i, row_values, row_marker, *marker_id_ptr);
        fill_result_row(right, result, &write_pos, row_values, row_marker, *marker_id_ptr);
        result->row_pointers[i + 1] = write_pos;
        i = i + 1;
    }
}

rc_t csr_csc_multiply(const CsrMatrix *left, const CscMatrix *right, CsrMatrix *result)
{
    rc_t status = STATUS_OK;
    if (left->cols != right->rows)
    {
        status = STATUS_DIM_MISMATCH_ERROR;
    }
    else
    {
        int n = left->cols;
        int m = left->rows;
        int p = right->cols;
        double *row_values = (double *)malloc((size_t)n * sizeof(double));
        int *row_marker = (int *)calloc((size_t)n, sizeof(int));
        int marker_id = 0;

        if (row_values == NULL || row_marker == NULL)
        {
            status = STATUS_MEMORY_ALLOC_ERROR;
        }
        else
        {
            int result_nnz = count_result_nnz(left, right, row_values, row_marker, &marker_id);
            status = csr_init(result, m, p, result_nnz);
            if (status == STATUS_OK)
            {
                fill_result_matrix(left, right, result, row_values, row_marker, &marker_id);
            }
        }
        free(row_values);
        free(row_marker);
    }
    return status;
}

rc_t dense_multiply(const DenseMatrix *left, const DenseMatrix *right, DenseMatrix *result)
{
    rc_t status = STATUS_OK;
    if (left->cols != right->rows)
    {
        status = STATUS_DIM_MISMATCH_ERROR;
    }
    else if (left->rows > MAX_DIM || right->cols > MAX_DIM)
    {
        status = STATUS_MATRIX_TOO_LARGE;
    }
    else
    {
        int m = left->rows;
        int n = left->cols;
        int p = right->cols;
        int i = 0;
        while (i < m)
        {
            int j = 0;
            while (j < p)
            {
                double sum = 0.0;
                int k = 0;
                while (k < n)
                {
                    sum = sum + left->data[i][k] * right->data[k][j];
                    k = k + 1;
                }
                result->data[i][j] = sum;
                j = j + 1;
            }
            i = i + 1;
        }
        result->rows = m;
        result->cols = p;
    }
    return status;
}
