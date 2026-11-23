#ifndef SIMULATION_H
#define SIMULATION_H

#include <stddef.h>
#include "status.h"

typedef struct
{
    double t1_min, t1_max;  /* интервал прихода заявок T1 */
    double t2_min, t2_max;  /* время обслуживания T2 */
    double p_return;        /* вероятность возврата в очередь P */
    int total_out;          /* сколько заявок должно покинуть систему */
    int report_step;        /* период промежуточного вывода */
} sim_params_t;

typedef struct
{
    rc_t rc;
    double total_time;
    double idle_time;
    double wait_sum;        /* суммарное время ожидания в очереди */
    double area;            /* интеграл длины очереди по времени */
    size_t entered;         /* новых заявок вошло в систему */
    size_t exited;          /* заявок покинуло систему */
    size_t activations;     /* срабатываний ОА */
    size_t max_len;         /* максимальная длина очереди */
} sim_result_t;

/* Абстракция очереди: одна и та же модель работает с массивом и списком */
typedef struct
{
    const char *name;
    void *impl;
    rc_t (*push)(void *, int, double);
    rc_t (*pop)(void *, int *, double *);
    size_t (*length)(void *);
} queue_if_t;

#include "static_queue.h"
#include "ll_queue.h"

queue_if_t make_static_if(static_queue_t *q);
queue_if_t make_list_if(dsc_t *d);

void set_default_params(sim_params_t *p);
sim_result_t simulate(queue_if_t *q, const sim_params_t *p, int report);
void print_summary(const sim_result_t *r, const sim_params_t *p);

#endif
