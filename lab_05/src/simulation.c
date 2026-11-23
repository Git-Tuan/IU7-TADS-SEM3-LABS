#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "simulation.h"
#include "generate_time.h"

static rc_t st_push(void *c, int id, double t) { return queue_st(c, id, t); }
static rc_t st_pop(void *c, int *id, double *t) { return queue_pop(c, id, t); }
static size_t st_len(void *c) { return length_st(c); }
static rc_t ll_push(void *c, int id, double t) { return queue_ll(c, id, t); }
static rc_t ll_pop(void *c, int *id, double *t) { return dequeue_ll(c, id, t); }
static size_t ll_len(void *c) { return length_ll(c); }

queue_if_t make_static_if(static_queue_t *q)
{
    queue_if_t r = { "статический массив (кольцевой)", q, st_push, st_pop, st_len };
    return r;
}

queue_if_t make_list_if(dsc_t *d)
{
    queue_if_t r = { "односвязный список", d, ll_push, ll_pop, ll_len };
    return r;
}

void set_default_params(sim_params_t *p)
{
    p->t1_min = 0.0;
    p->t1_max = 6.0;
    p->t2_min = 0.0;
    p->t2_max = 1.0;
    p->p_return = 0.8;
    p->total_out = 1000;
    p->report_step = 100;
}

sim_result_t simulate(queue_if_t *q, const sim_params_t *p, int report)
{
    sim_result_t r = { STATUS_OK, 0, 0, 0, 0, 0, 0, 0, 0 };

    double t = 0.0;
    double next_arrival = generate_time(p->t1_min, p->t1_max);
    double finish = 0.0;
    bool busy = false;
    int cur_id = 0;
    int next_id = 1;
    size_t stays = 0;               /* число выходов заявок из очереди в ОА */

    while (r.rc == STATUS_OK && r.exited < (size_t)p->total_out)
    {
        /* ближайшее событие: приход заявки или конец обслуживания.
           При равенстве времён сначала обрабатывается приход. */
        bool is_finish = busy && finish < next_arrival;
        double event = is_finish ? finish : next_arrival;
        double dt = event - t;

        r.area += (double)q->length(q->impl) * dt;
        if (!busy)
            r.idle_time += dt;
        t = event;

        bool report_now = false;

        if (is_finish)
        {
            busy = false;
            if (generate_time(0.0, 1.0) < p->p_return)
                r.rc = q->push(q->impl, cur_id, t);   /* новый цикл */
            else
            {
                r.exited++;
                report_now = (r.exited % (size_t)p->report_step == 0);
            }
        }
        else
        {
            r.rc = q->push(q->impl, next_id++, t);
            r.entered++;
            next_arrival = t + generate_time(p->t1_min, p->t1_max);
        }

        if (r.rc == STATUS_OK && q->length(q->impl) > r.max_len)
            r.max_len = q->length(q->impl);

        /* ОА свободен и есть заявка в очереди -> начинаем обслуживание */
        if (r.rc == STATUS_OK && !busy && q->length(q->impl) > 0)
        {
            double enter = 0.0;
            r.rc = q->pop(q->impl, &cur_id, &enter);
            if (r.rc == STATUS_OK)
            {
                r.wait_sum += t - enter;
                stays++;
                r.activations++;
                busy = true;
                finish = t + generate_time(p->t2_min, p->t2_max);
            }
        }

        if (report && report_now && r.rc == STATUS_OK)
        {
            printf("\n--- Покинуло систему %zu заявок (t = %.2f) ---\n", r.exited, t);
            printf("Текущая длина очереди:  %zu\n", q->length(q->impl));
            printf("Средняя длина очереди:  %.3f\n", t > 0 ? r.area / t : 0.0);
            printf("Вошло / вышло заявок:   %zu / %zu\n", r.entered, r.exited);
            printf("Среднее время в очереди: %.3f\n", stays ? r.wait_sum / stays : 0.0);
        }
    }

    r.total_time = t;
    /* r.wait_sum храним суммой; для среднего используем activations (= stays) */
    return r;
}

static void compare(const char *what, double calc, double real)
{
    printf("  %-28s расчёт %.2f, модель %.2f, расхождение %.2f%%\n",
           what, calc, real, calc > 0 ? fabs(calc - real) / calc * 100.0 : 0.0);
}

void print_summary(const sim_result_t *r, const sim_params_t *p)
{
    if (r->rc != STATUS_OK)
    {
        printf("\nОшибка моделирования (код %d): %s\n", (int)r->rc,
               r->rc == STATUS_PUSH_ERR
                   ? "переполнение очереди"
                   : "ошибка памяти/очереди");
    }
    else
    {
        printf("\n================ ИТОГИ ================\n");
        printf("Общее время моделирования: %.2f е.в.\n", r->total_time);
        printf("Время простоя аппарата: %.2f е.в.\n", r->idle_time);
        printf("Вошло в систему заявок: %zu\n", r->entered);
        printf("Вышло из системы заявок: %zu\n", r->exited);
        printf("Срабатываний ОА: %zu\n", r->activations);
        printf("Среднее время в очереди: %.3f е.в.\n",
               r->activations ? r->wait_sum / r->activations : 0.0);
        printf("Средняя длина очереди: %.3f\n", r->area / r->total_time);
        printf("Максимальная длина очереди: %zu\n", r->max_len);

        double avg_t1 = (p->t1_min + p->t1_max) / 2.0;
        double avg_t2 = (p->t2_min + p->t2_max) / 2.0;
        double calc_in = avg_t1 * r->entered;
        double calc_out = avg_t2 * r->activations;
        double busy = r->total_time - r->idle_time;

        printf("Сравнение с расчётом:\n");
        compare("по входу (ср.T1 * вошло):", calc_in, r->total_time);
        compare("по выходу (ср.T2 * сраб.):", calc_out, busy);
        printf("  (по выходу сравнивается с временем работы ОА = всё время - простой)\n");
    }
}
