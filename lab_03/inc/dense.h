/* dense.h */
#ifndef DENSE_H
#define DENSE_H

#include "matrix_types.h"
#include <stddef.h>

size_t dense_memory_bytes(const DenseMatrix *matrix);
void dense_print(const DenseMatrix *matrix);


#endif
