#include "physics.h"
#include "target.h"
#include "drone.h"
#include "truck.h"
#include "id.h"

#ifndef __SWARM_H__
#define __SWARM_H__

typedef struct {
  obj_id_t id;
  Target *target;
  Drone *drones[DRONES_PER_TRUCK];
} Swarm;

#endif
