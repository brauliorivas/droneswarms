#include "physics.h"
#include "timeh.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

bool equal_pos(Position *posa, Position *posb) {
  return posa->x == posb->x && posa->y == posb->y;
}

val distance(Position *start, Position *end) {
  return pow(pow(start->x - end->x, 2) + pow(start->y - end->y, 2), 0.5);
}

perc gas_spent(miliseconds time_flying) {
  return (time_flying / SEC_MSEC) * GAS_PER_SECOND;
}

Position *circular_fly(Position *current_position, Position *center, val radius,
                       speed s, miliseconds elapsed) {
  Position *new_pos;
  new_pos = malloc(sizeof(*new_pos));

  // 1. Current angle relative to the center
  val dx = current_position->x - center->x;
  val dy = current_position->y - center->y;
  val theta = atan2(dy, dx);

  // 2. Angular velocity: ω = v / r
  val omega = s / radius;

  // 3. Time in seconds
  val t = elapsed / SEC_MSEC;

  // 4. New angle
  val theta_new = theta + omega * t;

  // 5. New coordinates
  new_pos->x = center->x + radius * cos(theta_new);
  new_pos->y = center->y + radius * sin(theta_new);

  return new_pos;
}

Position *straight_fly(Position *current_position, Position *target_position,
                       speed s, miliseconds elapsed) {
  Position *new_pos;
  speed x_speed, y_speed;
  val costheta, sintheta, dist;

  new_pos = malloc(sizeof(*new_pos));
  dist = distance(current_position, target_position);
  costheta = (target_position->x - current_position->x) / dist;
  sintheta = (target_position->y - current_position->y) / dist;

  new_pos->x = current_position->x + (s * costheta) * (elapsed / SEC_MSEC);
  new_pos->y = current_position->y + (s * sintheta) * (elapsed / SEC_MSEC);

  /* Less than a meter of difference */
  if (fabs(new_pos->x - current_position->x) <= 1 &&
      fabs(new_pos->y - current_position->y) <= 1) {
    new_pos->x = target_position->x;
    new_pos->y = target_position->y;
  }

  return new_pos;
}
