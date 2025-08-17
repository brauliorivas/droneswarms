#include "physics.h"
#include "target.h"
#include "drone.h"
#include "truck.h"

#ifndef __SWARM_H__
#define __SWARM_H__

typedef struct {
  Target *target;
  Drone *drones[DRONES_PER_TRUCK];
} Swarm;

#endif
