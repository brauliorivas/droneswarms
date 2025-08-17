#include "id.h"
#include "physics.h"
#include "target.h"
#include <stdbool.h>

#ifndef __TRUCK_H__
#define __TRUCK_H__

#define DRONES_PER_TRUCK 5
#define N_ATTACK_DRONES 4
#define N_CAMERA_DRONES 1
#define CAMERA_DRONE_IDX 4

typedef struct {
  obj_id_t id;
  Position position;
  Target *targets[DRONES_PER_TRUCK];
} Truck;

void drone_truck(Truck *truck);
void command_truck(Target *targets, int *attack_targets, int *camera_targets, int ntargets);

#endif
