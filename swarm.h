#include "physics.h"
#include "queue.h"
#include "target.h"
#include "drone.h"
#include "truck.h"
#include "id.h"

#ifndef __SWARM_H__
#define __SWARM_H__

typedef struct swarm {
  obj_id_t id;
  Target *target;
  struct drone_head drone_list;
  LIST_ENTRY(swarm) swarm_l;
} Swarm;

LIST_HEAD(swarm_head, swarm);

#endif
