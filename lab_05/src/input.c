#include <stdio.h>
#include "input.h"
#include "status.h"

void clear_input(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

static rc_t read_double(const char *prompt, double *v)
{
    rc_t rc = STATUS_OK;
    printf("%s", prompt);

    if (scanf("%lf", v) != 1)
    {
        clear_input();
        rc = STATUS_INP_ERR;
    }

    return rc;
}

static rc_t read_base_params(sim_params_t *n)
{
    rc_t rc = read_double("T1 мин: ", &n->t1_min);
    if (rc == STATUS_OK)
        rc = read_double("T1 макс: ", &n->t1_max);
    if (rc == STATUS_OK)
        rc = read_double("T2 мин: ", &n->t2_min);
    if (rc == STATUS_OK)
        rc = read_double("T2 макс: ", &n->t2_max);
    if (rc == STATUS_OK)
        rc = read_double("P (0..1): ", &n->p_return);

    return rc;
}

static int params_valid(const sim_params_t *p)
{
    return p->t1_min >= 0.0 &&
           p->t1_max >= p->t1_min &&
           p->t2_min >= 0.0 &&
           p->t2_max >= p->t2_min &&
           p->p_return >= 0.0 &&
           p->p_return < 1.0 &&
           p->total_out > 0;
}

static void set_report_step(sim_params_t *p)
{
    p->report_step = p->total_out >= 10 ? p->total_out / 10 : 1;
    if (p->total_out == 1000)
        p->report_step = 100;
}

rc_t change_params(sim_params_t *p)
{
    sim_params_t n = *p;
    double cnt = 0.0;
    rc_t rc = read_base_params(&n);

    if (rc == STATUS_OK)
        rc = read_double("Сколько заявок должно выйти: ", &cnt);

    if (rc == STATUS_OK)
    {
        n.total_out = (int)cnt;
        if (!params_valid(&n))
            rc = STATUS_PARAM_ERR;
    }

    if (rc == STATUS_OK)
    {
        set_report_step(&n);
        *p = n;
    }

    return rc;
}

void print_change_result(rc_t rc, const sim_params_t *p)
{
    if (rc == STATUS_OK)
        printf("Параметры изменены (отчёт каждые %d заявок).\n", p->report_step);
    else if (rc == STATUS_INP_ERR)
        printf("Ошибка ввода, параметры не изменены.\n");
    else if (rc == STATUS_PARAM_ERR)
        printf("Некорректные данные, параметры не изменены (P в [0;1)).\n");
}

void print_params(const sim_params_t *p)
{
    printf("T1 = [%.2f; %.2f], T2 = [%.2f; %.2f], P = %.2f, заявок на выход: %d\n",
           p->t1_min, p->t1_max, p->t2_min, p->t2_max, p->p_return, p->total_out);
}
