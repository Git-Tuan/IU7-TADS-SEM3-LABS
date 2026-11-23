#include <time.h>
#include <stdlib.h>
#include "generate_time.h"

void init_seed(void)
{
    srand((unsigned int)time(NULL));
}

void set_seed(unsigned int seed)
{
    srand(seed);
}

double generate_time(double min_time, double max_time)
{
    return min_time + (max_time - min_time) * (rand() / (double)RAND_MAX);
}
