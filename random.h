#include "physics.h"
#include <stddef.h>

#ifndef __RANDOM_H__
#define __RANDOM_H__

int random_value(Range *range);

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void shuffle(int *array, size_t n);

#endif
