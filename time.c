#include "timeh.h"

miliseconds elapsed_time(struct timespec *end, struct timespec *start) {
  return (end->tv_sec - start->tv_sec) * SEC_MSEC +
         (end->tv_nsec - start->tv_nsec) / MSEC_NSEC;
}
