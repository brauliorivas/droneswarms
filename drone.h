#include "id.h"

#ifndef __DRONE_H__
#define __DRONE_H__

typedef enum {
  ATTACK,
  CAMERA,
} drone_type;

typedef struct {
  drone_type type;
  obj_id_t id;
} Drone;

void drone();

#endif
