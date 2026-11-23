#ifndef RUNNER_H
#define RUNNER_H

#include "simulation.h"

void compare_impl(const sim_params_t *p);
void run_list(const sim_params_t *p, int show_addr);
void run_static(const sim_params_t *p, int show_addr);
#endif
