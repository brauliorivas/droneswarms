#include "id.h"
#include "physics.h"

#ifndef __TARGET_H__
#define __TARGET_H__

typedef struct {
  Position pos;
  Stripe ensemble_zone;
  Stripe rensemble_zone;
  obj_id_t id;
} Target;

#endif
