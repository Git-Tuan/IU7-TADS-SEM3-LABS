#ifndef UI_H
#define UI_H

#include "app_context.h"
#include "errors.h"

void show_menu(void);
rc_t handle_manual_input(AppContext *context);
rc_t handle_random_generation(AppContext *context);
rc_t handle_show_matrices(AppContext *context, const DenseMatrix *dense_c);
rc_t handle_sparse_multiply(AppContext *context);
rc_t handle_dense_multiply(AppContext *context, DenseMatrix *dense_a, DenseMatrix *dense_b, DenseMatrix *dense_c);
rc_t handle_benchmark(AppContext *context);
rc_t handle_free_all(AppContext *context);

#endif
