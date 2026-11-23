#include "runner.h"
#include "generate_time.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SEED 12345u

void run_static(const sim_params_t *p, int show_addr)
{
    static static_queue_t q;
    init_queue_st(&q);
    queue_if_t qi = make_static_if(&q);

    set_show_addr_st(show_addr);
    set_seed(SEED);
    sim_result_t r = simulate(&qi, p, 1);
    set_show_addr_st(0);
    print_summary(&r, p);
    if (show_addr)
        printf("Массив непрерывный: фрагментации памяти нет, адреса идут подряд по кругу.\n");
}

void run_list(const sim_params_t *p, int show_addr)
{
    dsc_t d = { NULL, NULL, 0 };
    queue_if_t qi = make_list_if(&d);

    reset_ll_mem_stats();
    set_show_addr_ll(show_addr);
    set_seed(SEED);
    sim_result_t r = simulate(&qi, p, 1);
    set_show_addr_ll(0);
    print_summary(&r, p);
    print_ll_mem_stats();
    destroy_queue(&d);
}

/* Сравнение по времени и памяти: чистые операции push/pop без модели */
static double bench_static(int n, int reps)
{
    static static_queue_t q;
    init_queue_st(&q);
    clock_t s = clock();
    for (int k = 0; k < reps; k++)
    {
        for (int i = 0; i < n; i++)
            queue_st(&q, i, 0.0);
        for (int i = 0; i < n; i++)
            queue_pop(&q, NULL, NULL);
    }
    return (double)(clock() - s) / CLOCKS_PER_SEC;
}

static double bench_list(int n, int reps)
{
    dsc_t d = { NULL, NULL, 0 };
    clock_t s = clock();
    for (int k = 0; k < reps; k++)
    {
        for (int i = 0; i < n; i++)
            queue_ll(&d, i, 0.0);
        for (int i = 0; i < n; i++)
            dequeue_ll(&d, NULL, NULL);
    }
    double t = (double)(clock() - s) / CLOCKS_PER_SEC;
    destroy_queue(&d);
    return t;
}

void compare_impl(const sim_params_t *p)
{
    static_queue_t *sq = calloc(1, sizeof(*sq));
    dsc_t d = { NULL, NULL, 0 };

    if (sq)
    {
        init_queue_st(sq);

        queue_if_t a = make_static_if(sq);
        queue_if_t b = make_list_if(&d);

        set_seed(SEED);
        sim_result_t ra = simulate(&a, p, 0);

        set_seed(SEED);
        sim_result_t rb = simulate(&b, p, 0);

        printf("\nСравнение реализаций:\n");
        printf("Макс. длина очереди: массив %zu, список %zu\n",
               ra.max_len, rb.max_len);

        printf("Память: массив %zu байт, список %zu байт\n",
               sizeof(static_queue_t),
               rb.max_len * sizeof(ll_queue_t) + sizeof(dsc_t));

        int n = MAX_QUEUE_SIZE;
        int reps = 200;

        printf("Время (%d циклов по %d push/pop): массив %.3f с, список %.3f с\n",
               reps, n, bench_static(n, reps), bench_list(n, reps));

        destroy_queue(&d);
    }

    free(sq);
}
