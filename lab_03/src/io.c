#include "io.h"
#include "csc.h"
#include "csr.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int *rows;
    int *cols;
    double *values;
    int count;
    int capacity;
} TripletBuffer;

static rc_t triplet_buffer_init(TripletBuffer *buf, int capacity)
{
    rc_t status = STATUS_OK;
    buf->rows = malloc(capacity * sizeof(int));
    buf->cols = malloc(capacity * sizeof(int));
    buf->values = malloc(capacity * sizeof(double));
    buf->count = 0;
    buf->capacity = capacity;
    if (buf->rows == NULL || buf->cols == NULL || buf->values == NULL)
    {
        status = STATUS_MEMORY_ALLOC_ERROR;
    }
    return status;
}

static void triplet_buffer_free(TripletBuffer *buf)
{
    free(buf->rows);
    buf->rows = NULL;
    free(buf->cols);
    buf->cols = NULL;
    free(buf->values);
    buf->values = NULL;
    buf->count = 0;
    buf->capacity = 0;
}

static rc_t triplet_buffer_grow(TripletBuffer *buf)
{
    rc_t status = STATUS_OK;
    int new_cap = buf->capacity * 2;
    int *new_rows = realloc(buf->rows, new_cap * sizeof(int));
    int *new_cols = realloc(buf->cols, new_cap * sizeof(int));
    double *new_values = realloc(buf->values, new_cap * sizeof(double));
    if (new_rows == NULL || new_cols == NULL || new_values == NULL)
    {
        status = STATUS_MEMORY_ALLOC_ERROR;
    }
    else
    {
        buf->rows = new_rows;
        buf->cols = new_cols;
        buf->values = new_values;
        buf->capacity = new_cap;
    }
    return status;
}

static rc_t triplet_buffer_push(TripletBuffer *buf, int r, int c, double v)
{
    rc_t status = STATUS_OK;
    if (buf->count >= buf->capacity)
    {
        status = triplet_buffer_grow(buf);
    }
    if (status == STATUS_OK)
    {
        buf->rows[buf->count] = r;
        buf->cols[buf->count] = c;
        buf->values[buf->count] = v;
        buf->count = buf->count + 1;
    }
    return status;
}

static rc_t read_row_index(int rows, int *r, int *stop)
{
    rc_t status = STATUS_STRING_INPUT_ERROR;
    char prompt[96];
    snprintf(prompt, sizeof(prompt), "Строка [0..%d] (-1 — закончить ввод): ", rows - 1);
    *stop = 0;
    while (status != STATUS_OK && status != STATUS_INPUT_EOF)
    {
        status = read_int_once(prompt, r);
        if (status == STATUS_OK)
        {
            if (*r < 0)
            {
                *stop = 1;
            }
            else if (*r >= rows)
            {
                status = STATUS_INVALID_INDEX_ERROR;
            }
        }
        if (status != STATUS_OK && status != STATUS_INPUT_EOF)
        {
            print_error_msg(status);
        }
    }
    return status;
}

static rc_t read_column_index(int cols, int *c)
{
    rc_t status = STATUS_STRING_INPUT_ERROR;
    char prompt[64];
    snprintf(prompt, sizeof(prompt), "Столбец [0..%d]: ", cols - 1);
    while (status != STATUS_OK && status != STATUS_INPUT_EOF)
    {
        status = read_int_once(prompt, c);
        if (status == STATUS_OK && (*c < 0 || *c >= cols))
        {
            status = STATUS_INVALID_INDEX_ERROR;
        }
        if (status != STATUS_OK && status != STATUS_INPUT_EOF)
        {
            print_error_msg(status);
        }
    }
    return status;
}

static rc_t read_value(double *v)
{
    rc_t status = STATUS_STRING_INPUT_ERROR;
    while (status != STATUS_OK && status != STATUS_INPUT_EOF)
    {
        status = read_double_once("Значение (ненулевое вещественное число): ", v);
        if (status != STATUS_OK && status != STATUS_INPUT_EOF)
        {
            print_error_msg(status);
        }
    }
    return status;
}

static rc_t read_one_triplet(int rows, int cols, int *r, int *c, double *v, int *stop)
{
    rc_t status = read_row_index(rows, r, stop);
    if (status == STATUS_OK && !*stop)
    {
        status = read_column_index(cols, c);
    }
    if (status == STATUS_OK && !*stop)
    {
        status = read_value(v);
    }
    return status;
}

static rc_t collect_triplets(int rows, int cols, TripletBuffer *buf)
{
    rc_t status = STATUS_OK;
    int stop = 0;
    size_t total_positions = (size_t)rows * (size_t)cols;
    unsigned char *seen = calloc(total_positions, 1);

    if (seen == NULL)
    {
        status = STATUS_MEMORY_ALLOC_ERROR;
    }
    else
    {
        printf("Вводите тройки: <строка> <столбец> <значение>.\n");
        printf("Индексы 0-базовые. Для окончания введите -1 в строке.\n");
        printf("Максимум ненулевых: %zu (равно rows * cols).\n", total_positions);

        while (!stop && status == STATUS_OK)
        {
            int r = 0;
            int c = 0;
            double v = 0.0;
            status = read_one_triplet(rows, cols, &r, &c, &v, &stop);

            if (status == STATUS_OK && !stop)
            {
                if (v == 0.0)
                {
                    printf("Нулевое значение не сохраняется.\n");
                }
                else if (seen[(size_t)r * cols + c])
                {
                    printf("Позиция (%d, %d) уже была введена — дубликат пропущен.\n", r, c);
                }
                else
                {
                    seen[(size_t)r * cols + c] = 1;
                    status = triplet_buffer_push(buf, r, c, v);
                }
            }
        }
        free(seen);
    }
    return status;
}

static rc_t read_sparse_triplets(int rows, int cols, TripletBuffer *buf)
{
    rc_t status = triplet_buffer_init(buf, 64);
    if (status == STATUS_OK)
    {
        status = collect_triplets(rows, cols, buf);
    }
    if (status == STATUS_OK && buf->count == 0)
    {
        printf("Пустой ввод: ни одного ненулевого элемента не задано.\n");
        status = STATUS_EMPTY_INPUT_ERROR;
    }
    return status;
}

/* Матрица A: запрашиваем оба размера. */
rc_t input_coordinate_csr(CsrMatrix *matrix)
{
    int rows = 0;
    int cols = 0;
    TripletBuffer buf = {NULL, NULL, NULL, 0, 0};
    rc_t status = read_int("Число строк матрицы A: ", 1, MAX_DIM, &rows);

    if (status == STATUS_OK)
    {
        status = read_int("Число столбцов матрицы A: ", 1, MAX_DIM, &cols);
    }
    if (status == STATUS_OK)
    {
        status = read_sparse_triplets(rows, cols, &buf);
    }
    if (status == STATUS_OK)
    {
        status = csr_from_triplets(matrix, rows, cols, buf.count, buf.rows, buf.cols, buf.values);
    }
    triplet_buffer_free(&buf);
    return status;
}

rc_t input_coordinate_csc(CscMatrix *matrix, int rows)
{
    int cols = 0;
    TripletBuffer buf = {NULL, NULL, NULL, 0, 0};
    rc_t status = STATUS_OK;

    printf("Матрица B имеет %d строк (равно числу столбцов A).\n", rows);
    status = read_int("Число столбцов матрицы B: ", 1, MAX_DIM, &cols);
    if (status == STATUS_OK)
    {
        status = read_sparse_triplets(rows, cols, &buf);
    }
    if (status == STATUS_OK)
    {
        status = csc_from_triplets(matrix, rows, cols, buf.count, buf.rows, buf.cols, buf.values);
    }
    triplet_buffer_free(&buf);
    return status;
}

static rc_t build_density_mask(unsigned char *mask, int rows, int cols, double density, int *nnz_out) 
{
    rc_t status = STATUS_OK;
    int total = rows * cols;
    int target = (int)(density * total + 0.5);

    if (target > total) { target = total; }

    memset(mask, 0, (size_t)total);


    int *indices = malloc((size_t)total * sizeof(int));
    if (indices == NULL) 
    {
        status = STATUS_MEMORY_ALLOC_ERROR;
    } 
    else 
    {
        int i = 0;
        while (i < total) 
        { 
            indices[i] = i; 
            i = i + 1; 
        }
        i = 0;
        while (i < target) 
        {
            int j = i + rand() % (total - i);
            int tmp = indices[i];
            indices[i] = indices[j];
            indices[j] = tmp;
            mask[indices[i]] = 1;
            i = i + 1;
        }
        free(indices);
        *nnz_out = target;
    }
    return status;
}

static double random_nonzero_value(void)
{
    double value = ((double)rand() / (double)RAND_MAX) * 20.0 - 10.0;
    if (value == 0.0)
    {
        value = 1.0;
    }
    return value;
}

static void fill_csr_from_mask(const unsigned char *mask, int rows, int cols, CsrMatrix *matrix)
{
    int pos = 0;
    int i = 0;
    matrix->row_pointers[0] = 0;
    while (i < rows)
    {
        int j = 0;
        while (j < cols)
        {
            if (mask[(size_t)i * cols + j])
            {
                matrix->values[pos] = random_nonzero_value();
                matrix->col_indices[pos] = j;
                pos = pos + 1;
            }
            j = j + 1;
        }
        matrix->row_pointers[i + 1] = pos;
        i = i + 1;
    }
}

static void fill_csc_from_mask(const unsigned char *mask, int rows, int cols, CscMatrix *matrix)
{
    int pos = 0;
    int j = 0;
    matrix->col_pointers[0] = 0;
    while (j < cols)
    {
        int i = 0;
        while (i < rows)
        {
            if (mask[(size_t)i * cols + j])
            {
                matrix->values[pos] = random_nonzero_value();
                matrix->row_indices[pos] = i;
                pos = pos + 1;
            }
            i = i + 1;
        }
        matrix->col_pointers[j + 1] = pos;
        j = j + 1;
    }
}

rc_t generate_random_csr(CsrMatrix *matrix, int rows, int cols, double density)
{
    rc_t status = STATUS_OK;
    int nnz = 0;

    if (density < 0.0 || density > 1.0)
    {
        status = STATUS_INVALID_DENSITY_ERROR;
    }
    else
    {
        unsigned char *mask = malloc((size_t)rows * (size_t)cols);
        if (mask == NULL)
        {
            status = STATUS_MEMORY_ALLOC_ERROR;
        }
        else
        {
            build_density_mask(mask, rows, cols, density, &nnz);
            status = csr_init(matrix, rows, cols, nnz);
            if (status == STATUS_OK)
            {
                fill_csr_from_mask(mask, rows, cols, matrix);
            }
            free(mask);
        }
    }
    return status;
}

rc_t generate_random_csc(CscMatrix *matrix, int rows, int cols, double density)
{
    rc_t status = STATUS_OK;
    int nnz = 0;

    if (density < 0.0 || density > 1.0)
    {
        status = STATUS_INVALID_DENSITY_ERROR;
    }
    else
    {
        unsigned char *mask = (unsigned char *)malloc((size_t)rows * (size_t)cols);
        if (mask == NULL)
        {
            status = STATUS_MEMORY_ALLOC_ERROR;
        }
        else
        {
            build_density_mask(mask, rows, cols, density, &nnz);
            status = csc_init(matrix, rows, cols, nnz);
            if (status == STATUS_OK)
            {
                fill_csc_from_mask(mask, rows, cols, matrix);
            }
            free(mask);
        }
    }
    return status;
}
