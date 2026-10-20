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
    double *values;    /* A  */
    int *col_indices;  /* JA */
    int *row_pointers; /* IA, длина rows+1 */
} CsrMatrix;

typedef struct
{
    int rows;
    int cols;
    int nnz;
    double *values;    /* A  */
    int *row_indices;  /* IA */
    int *col_pointers; /* JA, длина cols+1 */
} CscMatrix;

#endif
