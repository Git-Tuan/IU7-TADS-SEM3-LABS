/* csr.h */
#ifndef CSR_H
#define CSR_H

#include "errors.h"
#include "matrix_types.h"
#include <stddef.h>

rc_t csr_init(CsrMatrix *matrix, int rows, int cols, int nnz);
void csr_free(CsrMatrix *matrix);
size_t csr_memory_bytes(const CsrMatrix *matrix);
rc_t csr_to_dense(const CsrMatrix *src, DenseMatrix *dst);
rc_t csr_from_triplets(CsrMatrix *matrix, int rows, int cols, int count, const int *trows, const int *tcols,
                       const double *tvalues);
void csr_print_arrays(const CsrMatrix *matrix);
void csr_print_dense(const CsrMatrix *matrix);

#endif
