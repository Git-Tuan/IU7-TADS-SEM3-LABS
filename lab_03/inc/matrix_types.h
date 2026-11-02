#ifndef MATRIX_TYPES_H
#define MATRIX_TYPES_H

#define MAX_DIM 1000

typedef struct
{
    int rows;
    int cols;
    double data[MAX_DIM][MAX_DIM];
} DenseMatrix;

typedef struct
{
    int rows;
    int cols;
    int nnz;
    double *values;
    int *col_indices;
    int *row_pointers;
} CsrMatrix;

typedef struct
{
    int rows;
    int cols;
    int nnz;
    double *values;
    int *row_indices;
    int *col_pointers;
} CscMatrix;

#endif
