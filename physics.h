#include "timeh.h"
#include <stdbool.h>
#include <stdint.h>

#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#define MIN_X 0
#define MAX_X 1000
#define MIN_Y 0
#define MAX_Y 1000
#define DELTA 50

#define SPEED 1 /* 1 [m/s] */
#define METERS_PER_GALON 12
#define GAS_PER_SECOND 1 /* one level gas every second */
#define RADIUS 5

typedef double val;
typedef val speed;
typedef double perc;

typedef struct {
  val x;
  val y;
} Position;

typedef struct {
} Vector;

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

extern Stripe ensemble_zone;
extern Stripe defense_zone;
extern Stripe resemble_zone;

bool equal_pos(Position *posa, Position *posb);
val distance(Position *start, Position *end);

perc gas_spent(miliseconds time_flying);

Position *straight_fly(Position *current_position, Position *target_position,
                       speed s, miliseconds t);
Position *circular_fly(Position *current_position, Position *center,
                       val radius, speed s, miliseconds t);

#endif
