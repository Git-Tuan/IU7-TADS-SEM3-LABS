#include "app_context.h"
#include "csc.h"
#include "csr.h"
#include <string.h>

void context_init(AppContext *ctx)
{
    memset(ctx, 0, sizeof(AppContext));
}

void context_free(AppContext *ctx)
{
    csr_free(&ctx->csr_a);
    csc_free(&ctx->csc_b);
    csr_free(&ctx->csr_c);
    ctx->has_sparse_data = 0;
    ctx->has_dense_data = 0;
}
