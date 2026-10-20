/* dense.c */
#include "dense.h"
#include <stdio.h>

size_t dense_memory_bytes(const DenseMatrix *matrix)
{
    return (size_t)matrix->rows * (size_t)matrix->cols * sizeof(double);
}
