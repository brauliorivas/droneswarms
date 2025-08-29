#include "drone.h"
#include "message.h"
#include "network.h"
#include "physics.h"
#include "target.h"
#include "timeh.h"
#include <bits/pthreadtypes.h>
#include <bits/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define DRONE "DRONE"

typedef struct {
  /* Identification */
  obj_id_t id;
  /* namespace for logging */
  char *namespace;

  /* Communication data */
  int truck_port;
  int server_fd;

  /* Position protected by binary semaphore */
  Position position;
  sem_t sem_position;

  /* Target protected by binary semaphore */
  Target target;
  sem_t sem_target;

  /* Start fly */
  sem_t start_fly;

  /* Complete ensemble */
  bool ensemble_complete;

  /* Gas */
  perc galons;

  /* */
} DroneData;

static void drone_sleep(struct timespec *start, struct timespec *end) {
  static struct timespec req;
  req.tv_nsec = SLEEP_NSEC;
  req.tv_sec = SLEEP_SEC;

  clock_gettime(CLOCK_MONOTONIC, start);
  nanosleep(&req, NULL);
  clock_gettime(CLOCK_MONOTONIC, end);
}

static void write_position(DroneData *drone_data, Position *position) {
  vprint_log(drone_data->namespace,
             "Drone is in the current position. x: %lf y: %lf", position->x,
             position->y);
}

static void *navigation_ctrl(void *targs) {
  DroneData *drone = targs;
  Position *new_pos, ensemble_center, resemble_center;
  struct timespec start, end;
  miliseconds delta_t;
  int truck_fd;

  print_log(drone->namespace, "Connecting to truck");
  if ((truck_fd = connect_to_server(drone->truck_port)) == -1) {
    print_error(drone->namespace, "Can't connect to truck");
    exit(1);
  }

  if (read(truck_fd, &drone->position, sizeof(Position)) == -1) {
    print_error(drone->namespace, "Can't read position from truck");
    exit(1);
  }
  if (read(truck_fd, &drone->target, sizeof(Target)) == -1) {
    print_error(drone->namespace, "Can't read target");
    exit(1);
  }

  vprint_log(DRONE, "target id: %lu drone id: %lu", drone->target.id,
             drone->id);

  close(truck_fd);

  ensemble_center.x = drone->target.pos.x;
  ensemble_center.y = ensemble_zone.range.x1 +
                      (ensemble_zone.range.x2 - ensemble_zone.range.x1) / 2;
  resemble_center.x = drone->target.pos.x;
  // this time won't be to exact center because may need to change swarm
  resemble_center.y = resemble_zone.range.x1;

  sem_wait(&drone->start_fly);

  // fly towards ensemble zone center
  while (1) {
    drone_sleep(&start, &end);
    new_pos = straight_fly(&drone->position, &ensemble_center, SPEED,
                           elapsed_time(&end, &start));
    sem_wait(&drone->sem_position);
    drone->position.x = new_pos->x;
    drone->position.y = new_pos->y;
    sem_post(&drone->sem_position);
    write_position(drone, new_pos);
    free(new_pos);

    if (equal_pos(&drone->position, &ensemble_center))
      break;
  }

  // circular fly while waiting for ensemble to complete
  while (1) {
    drone_sleep(&start, &end);
    new_pos = circular_fly(&drone->position, &ensemble_center, RADIUS, SPEED,
                           elapsed_time(&end, &start));
    sem_wait(&drone->sem_position);
    drone->position.x = new_pos->x;
    drone->position.y = new_pos->y;
    sem_post(&drone->sem_position);
    write_position(drone, new_pos);
    free(new_pos);

    if (drone->ensemble_complete)
      break;
  }

  /*
   * fly directly to target.
   * But it's the same as flying towards resemble zone's center
   * */
  while (1) {
    drone_sleep(&start, &end);
    new_pos = straight_fly(&drone->position, &resemble_center, SPEED,
                           elapsed_time(&end, &start));
    sem_wait(&drone->sem_position);
    drone->position.x = new_pos->x;
    drone->position.y = new_pos->y;
    sem_post(&drone->sem_position);
    write_position(drone, new_pos);
    free(new_pos);

    if (equal_pos(&drone->position, &resemble_center))
      break;
  }

  /* Fly towards target, that may change at any moment */
  while (1) {
    drone_sleep(&start, &end);

    sem_wait(&drone->sem_target);
    new_pos = straight_fly(&drone->position, &drone->target.pos, SPEED,
                           elapsed_time(&end, &start));
    sem_post(&drone->sem_target);

    sem_wait(&drone->sem_position);
    drone->position.x = new_pos->x;
    drone->position.y = new_pos->y;
    sem_post(&drone->sem_position);
    write_position(drone, new_pos);
    free(new_pos);

    if (equal_pos(&drone->position, &resemble_center))
      break;
  }

  return NULL;
}

static void *fuel_ctrl(void *targs) {
  DroneData *drone = targs;
  struct timespec start, end;
  Position old, new;
  val d;
  perc spent;
  while (1) {
    sem_wait(&drone->sem_position);
    old.x = drone->position.x;
    old.y = drone->position.y;
    sem_post(&drone->sem_position);

    drone_sleep(&start, &end);

    sem_wait(&drone->sem_position);
    new.x = drone->position.x;
    new.y = drone->position.y;
    sem_post(&drone->sem_position);

    d = distance(&old, &new);

    if (d == 0) {
      spent = gas_spent(elapsed_time(&end, &start));
    } else {
      spent = d / METERS_PER_GALON; // galons spent
    }

    drone->galons -= spent;
    drone->galons = drone->galons > 0 ? drone->galons : 0;

    if (drone->galons == 0) {
      break;
    }
  }

  /* auto destroy */

  return NULL;
}

static void *weapon_ctrl(void *targs) {
  DroneData *drone = targs;
  return NULL;
}

static void *camera_ctrl(void *targs) {
  DroneData *drone = targs;
  return NULL;
}

static void *network_ctrl(void *targs) {
  DroneData *drone = targs;
  int command_fd;
  Message message;
  MessageType message_type;

  print_log(drone->namespace, "Connecting to command");
  if ((command_fd = connect_to_server(SERVER_PORT)) == -1) {
    print_error(drone->namespace, "Can't connect to command");
    exit(1);
  }

  print_log(drone->namespace, "Drone connected to command");

  while (1) {
    if (read(command_fd, &message, sizeof(message)) == -1) {
      print_error(drone->namespace, "Error reading message");
    }

    message_type = get_message_type(message);

    switch (message_type) {
    case CHECK:
      break;
    case CHANGE_TARGET:
      sem_wait(&drone->sem_target);
      sem_post(&drone->sem_target);
      break;
    case ENSEMBLE_COMPLETE:
      drone->ensemble_complete = true;
      break;
    case START_FLY:
      sem_post(&drone->start_fly);
      break;
    }
  }

  close(command_fd);
  return NULL;
}

void drone(Drone *drone_data, int truck_port) {
  DroneData drone;
  pthread_t systems[N_SYSTEMS];
  int i;

  drone.id = drone_data->id;
  asprintf(&drone.namespace, "%s:%lu:%s", DRONE, drone_data->id,
           drone_data->type == ATTACK ? "ATTACK" : "CAMERA");
  drone.truck_port = truck_port;

  sem_init(&drone.sem_position, 0, 1);
  sem_init(&drone.sem_target, 0, 1);

  sem_init(&drone.start_fly, 0, 0);

  drone.ensemble_complete = false;
  drone.galons = 100;

  print_log(drone.namespace, "Drone has started its execution");

  if (pthread_create(&systems[NAVIGATION_IDX], NULL, navigation_ctrl, &drone) !=
      0) {
    print_error(drone.namespace, "Can't start navigation system");
    exit(1);
  }

  if (pthread_create(&systems[FUEL_IDX], NULL, fuel_ctrl, NULL) != 0) {
    print_error(drone.namespace, "Can't start fuel system");
    exit(1);
  }

  if (pthread_create(&systems[NETWORK_IDX], NULL, network_ctrl, &drone) != 0) {
    print_error(drone.namespace, "Can't start networking system");
    exit(1);
  }

  switch (drone_data->type) {
  case ATTACK:
    if (pthread_create(&systems[WEAPON_IDX], NULL, weapon_ctrl, NULL) != 0) {
      print_error(drone.namespace, "Can't start weapons system");
      exit(1);
    }
    break;
  case CAMERA:
    if (pthread_create(&systems[CAMERA_IDX], NULL, camera_ctrl, NULL) != 0) {
      print_error(drone.namespace, "Can't start camera system");
      exit(1);
    }
    break;
  default:
    print_error(DRONE, "Unrecognized drone type");
  }

  print_log(drone.namespace, "Drone has started all its systems, ATTACKING!!!");
  free(drone_data);

  for (i = 0; i < N_SYSTEMS; i++) {
    if (pthread_join(systems[i], NULL) != 0) {
      print_error(drone.namespace, "Error waiting for system to finish");
      exit(1);
    }
  }

  exit(0);
}
