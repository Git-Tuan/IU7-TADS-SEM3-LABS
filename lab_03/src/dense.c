/* dense.c */
#include "dense.h"
#include <stdio.h>

size_t dense_memory_bytes(const DenseMatrix *matrix)
{
    return (size_t)matrix->rows * (size_t)matrix->cols * sizeof(double);
}

void dense_print(const DenseMatrix *matrix)
{
    if (matrix->rows > 30 || matrix->cols > 30)
    {
        printf("Матрица слишком большая для вывода в плотном виде\n");
    }
    else
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
}
