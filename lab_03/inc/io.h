/* io.h */
#ifndef IO_H
#define IO_H

#include "errors.h"
#include "matrix_types.h"

rc_t input_coordinate_csr(CsrMatrix *matrix);
rc_t input_coordinate_csc(CscMatrix *matrix);
rc_t generate_random_csr(CsrMatrix *matrix, int rows, int cols, double density);
rc_t generate_random_csc(CscMatrix *matrix, int rows, int cols, double density);

#endif
