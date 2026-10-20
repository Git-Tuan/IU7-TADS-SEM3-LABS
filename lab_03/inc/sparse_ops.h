/* sparse_ops.h */
#ifndef SPARSE_OPS_H
#define SPARSE_OPS_H

#include "errors.h"
#include "matrix_types.h"

rc_t csr_csc_multiply(const CsrMatrix *left, const CscMatrix *right, CsrMatrix *result);
rc_t dense_multiply(const DenseMatrix *left, const DenseMatrix *right, DenseMatrix *result);

#endif
