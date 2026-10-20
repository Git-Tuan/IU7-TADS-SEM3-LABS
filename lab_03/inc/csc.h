/* csc.h */
#ifndef CSC_H
#define CSC_H

#include "errors.h"
#include "matrix_types.h"
#include <stddef.h>

rc_t csc_init(CscMatrix *matrix, int rows, int cols, int nnz);
void csc_free(CscMatrix *matrix);
size_t csc_memory_bytes(const CscMatrix *matrix);
rc_t csc_to_dense(const CscMatrix *src, DenseMatrix *dst);
rc_t csc_from_triplets(CscMatrix *matrix, int rows, int cols, int count, const int *trows, const int *tcols,
                       const double *tvalues);
void csc_print_arrays(const CscMatrix *matrix);
void csc_print_dense(const CscMatrix *matrix);

#endif
