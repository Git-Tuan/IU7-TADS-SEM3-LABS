#include "benchmark.h"
#include "csc.h"
#include "csr.h"
#include "dense.h"
#include "io.h"
#include "sparse_ops.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct
{
    CsrMatrix csr_a;
    CscMatrix csc_b;
    CsrMatrix csr_c;
    DenseMatrix *dense_a;
    DenseMatrix *dense_b;
    DenseMatrix *dense_c;
} BenchRun;


static void free_benchmark_buffers(DenseMatrix *dense_a, DenseMatrix *dense_b, DenseMatrix *dense_c, CsrMatrix *csr_c)
{
    free(dense_a);
    free(dense_b);
    free(dense_c);
    csr_free(csr_c);
    free(csr_c);
}

static void print_benchmark_results(const CsrMatrix *csr_a, const CscMatrix *csc_b, const CsrMatrix *csr_c,
    double t_sparse, double t_dense, size_t mem_sparse, size_t mem_dense)
{
    printf("Размер A: %dx%d, nnz=%d\n", csr_a->rows, csr_a->cols, csr_a->nnz);
    printf("Размер B: %dx%d, nnz=%d\n", csc_b->rows, csc_b->cols, csc_b->nnz);
    printf("Размер C: %dx%d, nnz=%d\n", csr_c->rows, csr_c->cols, csr_c->nnz);
    print_separator();
    printf("Разреженный алгоритм: %.6f с, память %zu байт\n", t_sparse, mem_sparse);
    printf("Плотный алгоритм:     %.6f с, память %zu байт\n", t_dense, mem_dense);
    print_separator();
    if (t_sparse > 0.0)
    {
        printf("Ускорение по времени: %.3f\n", t_dense / t_sparse);
    }
    if (mem_sparse > 0)
    {
        printf("Отношение памяти: %.3f\n", (double)mem_dense / (double)mem_sparse);
    }
}

rc_t compare_single_run(const CsrMatrix *csr_a, const CscMatrix *csc_b)
{
    rc_t status = STATUS_OK;
    DenseMatrix *dense_a = (DenseMatrix *)malloc(sizeof(DenseMatrix));
    DenseMatrix *dense_b = (DenseMatrix *)malloc(sizeof(DenseMatrix));
    DenseMatrix *dense_c = (DenseMatrix *)malloc(sizeof(DenseMatrix));
    CsrMatrix *csr_c = (CsrMatrix *)calloc(1, sizeof(CsrMatrix));

    if (dense_a == NULL || dense_b == NULL || dense_c == NULL || csr_c == NULL)
    {
        status = STATUS_MEMORY_ALLOC_ERROR;
    }
    else
    {
        rc_t s1 = csr_to_dense(csr_a, dense_a);
        rc_t s2 = csc_to_dense(csc_b, dense_b);
        if (s1 != STATUS_OK || s2 != STATUS_OK)
        {
            status = STATUS_MATRIX_TOO_LARGE;
        }
        if (status == STATUS_OK)
        {
            double t0 = get_time_seconds();
            rc_t s3 = csr_csc_multiply(csr_a, csc_b, csr_c);
            double t1 = get_time_seconds();
            double t2 = get_time_seconds();
            rc_t s4 = dense_multiply(dense_a, dense_b, dense_c);
            double t3 = get_time_seconds();

            if (s3 != STATUS_OK)
            {
                status = s3;
            }
            else if (s4 != STATUS_OK)
            {
                status = s4;
            }
            else
            {
                size_t mem_csr = csr_memory_bytes(csr_a) + csc_memory_bytes(csc_b) + csr_memory_bytes(csr_c);
                size_t mem_dense = dense_memory_bytes(dense_a) + dense_memory_bytes(dense_b) + dense_memory_bytes(dense_c);
                print_benchmark_results(csr_a, csc_b, csr_c, t1 - t0, t3 - t2, mem_csr, mem_dense);
            }
        }
        free_benchmark_buffers(dense_a, dense_b, dense_c, csr_c);
    }
    return status;
}

static rc_t bench_run_init(BenchRun *run, int rows, int inner, int cols, double density)
{
    rc_t status = STATUS_OK;
    memset(run, 0, sizeof(BenchRun));
    run->dense_a = (DenseMatrix *)malloc(sizeof(DenseMatrix));
    run->dense_b = (DenseMatrix *)malloc(sizeof(DenseMatrix));
    run->dense_c = (DenseMatrix *)malloc(sizeof(DenseMatrix));
    if (run->dense_a == NULL || run->dense_b == NULL || run->dense_c == NULL)
    {
        status = STATUS_MEMORY_ALLOC_ERROR;
    }
    if (status == STATUS_OK)
    {
        status = generate_random_csr(&run->csr_a, rows, inner, density);
    }
    if (status == STATUS_OK)
    {
        status = generate_random_csc(&run->csc_b, inner, cols, density);
    }
    if (status == STATUS_OK)
    {
        status = csr_to_dense(&run->csr_a, run->dense_a);
    }
    if (status == STATUS_OK)
    {
        status = csc_to_dense(&run->csc_b, run->dense_b);
    }
    return status;
}

static void bench_run_free(BenchRun *run)
{
    csr_free(&run->csr_a);
    csc_free(&run->csc_b);
    csr_free(&run->csr_c);
    free(run->dense_a);
    run->dense_a = NULL;
    free(run->dense_b);
    run->dense_b = NULL;
    free(run->dense_c);
    run->dense_c = NULL;
}

static void bench_run_execute(BenchRun *run, double *t_sparse, double *t_dense, size_t *mem_sparse, size_t *mem_dense)
{
    double t0 = get_time_seconds();
    csr_csc_multiply(&run->csr_a, &run->csc_b, &run->csr_c);
    double t1 = get_time_seconds();
    double t2 = get_time_seconds();
    dense_multiply(run->dense_a, run->dense_b, run->dense_c);
    double t3 = get_time_seconds();

    *t_sparse = t1 - t0;
    *t_dense = t3 - t2;
    *mem_sparse = csr_memory_bytes(&run->csr_a) + csc_memory_bytes(&run->csc_b) + csr_memory_bytes(&run->csr_c);
    *mem_dense = dense_memory_bytes(run->dense_a) + dense_memory_bytes(run->dense_b) + dense_memory_bytes(run->dense_c);
}

rc_t run_benchmark_suite(int rows, int inner, int cols, double density_from, double density_to, double density_step)
{
    rc_t status = STATUS_OK;
    double density = density_from;

    printf("\n");
    print_separator();
    printf("Бенчмарк: A(%dx%d) * B(%dx%d)\n", rows, inner, inner, cols);
    printf("Плотность от %.3f до %.3f с шагом %.3f\n", density_from, density_to, density_step);
    print_separator();
    printf("%-10s %-14s %-14s %-14s %-14s %-14s\n", "Плотн.", "nnz(A)+nnz(B)", "t_sparse,с", "t_dense,с",
           "mem_sparse,Б", "mem_dense,Б");
    print_separator();

    while (density <= density_to + 1e-9)
    {
        BenchRun run;
        rc_t step = bench_run_init(&run, rows, inner, cols, density);
        if (step != STATUS_OK)
        {
            printf("Ошибка генерации при плотности %.3f\n", density);
            status = step;
            bench_run_free(&run);
        }
        else
        {
            double t_sparse = 0.0;
            double t_dense = 0.0;
            size_t mem_sparse = 0;
            size_t mem_dense = 0;
            bench_run_execute(&run, &t_sparse, &t_dense, &mem_sparse, &mem_dense);
            printf("%-10.3f %-14d %-14.6f %-14.6f %-14zu %-14zu\n", density, run.csr_a.nnz + run.csc_b.nnz, t_sparse,
                   t_dense, mem_sparse, mem_dense);
            bench_run_free(&run);
        }
        density = density + density_step;
    }
    print_separator();
    return status;
}
