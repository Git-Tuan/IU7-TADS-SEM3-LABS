/* benchmark.h */
#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "errors.h"
#include "matrix_types.h"

rc_t compare_single_run(const CsrMatrix *csr_a, const CscMatrix *csc_b);
rc_t run_benchmark_suite(int rows, int inner, int cols, double density_from, double density_to, double density_step);

#endif
