#include <stdint.h>

#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#define MIN_X 0
#define MAX_X 1000
#define MIN_Y 0
#define MAX_Y 1000

typedef uint64_t val;

typedef struct {
  val x;
  val y;
} Position;

typedef struct {
  val x1;
  val x2;
} Range;

typedef enum {
  VERTICAL,
  HORIZONTAL,
} StripeType;

typedef struct {
  Range range;
  StripeType type;
} Stripe;

#endif
