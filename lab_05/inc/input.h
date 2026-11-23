#ifndef INPUT_H
#define INPUT_H

#include "simulation.h"
#include "status.h"

void print_params(const sim_params_t *p);
rc_t change_params(sim_params_t *p);
void print_change_result(rc_t rc, const sim_params_t *p);
void clear_input(void);

#endif
