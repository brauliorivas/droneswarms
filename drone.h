#include "id.h"
#include "target.h"

#ifndef __DRONE_H__
#define __DRONE_H__

#define N_SYSTEMS 4
#define NAVIGATION_IDX 0
#define FUEL_IDX 1
#define WEAPON_IDX 2
#define CAMERA_IDX 2
#define NETWORK_IDX 3

typedef enum {
  ATTACK,
  CAMERA,
} drone_type;

typedef struct {
  drone_type type;
  obj_id_t id;
} Drone;

void drone(Drone *drone_data, int conn_fd);

#endif
