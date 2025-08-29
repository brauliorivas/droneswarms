#ifndef __TIMEH_H__
#define __TIMEH_H__

#include <stdint.h>
#include <time.h>
#define MSEC_NSEC 1000000

#define SLEEP_NSEC 10 * MSEC_NSEC 
#define SLEEP_SEC 0

#define SEC_MSEC 1000

typedef double miliseconds;

miliseconds elapsed_time(struct timespec *end, struct timespec *start);


#endif
