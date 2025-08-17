#include "random.h"
#include "physics.h"
#include <stdlib.h>

int random_value(Range *range) {
  int r = rand();
  if (range == NULL) {
    return r;
  }
  return range->x1 + (r % (range->x2 - range->x1));
}

void shuffle(int *array, size_t n) {
  Range random_idx = {0, n};
  size_t i, j;
  for (i = 0; i < n; i++) {
    j = random_value(&random_idx); 
    int t = array[j];
    array[j] = array[i];
    array[i] = t;
  }
}
