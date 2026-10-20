#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include "matrix_types.h"

/*
 * Плотные матрицы вынесены в отдельную структуру уровня файла
 * (см. app_context.c) из-за размера: 3 × 8 МБ не помещаются на стек.
 * Разреженные матрицы и флаги — обычные поля контекста,
 * контекст создаётся локально в main и передаётся по указателю.
 */
typedef struct
{
    CsrMatrix csr_a;
    CscMatrix csc_b;
    CsrMatrix csr_c;

    int has_sparse_data;
    int has_dense_data;
} AppContext;

void context_init(AppContext *ctx);
void context_free(AppContext *ctx);

#endif
