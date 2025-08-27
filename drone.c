#include "drone.h"
#include "message.h"
#include "network.h"
#include "physics.h"
#include "target.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DRONE "DRONE"

typedef struct {
  obj_id_t id;
  int truck_port;
  int server_fd;
  Position position;
  Target target;
  char *namespace;
} DroneData;

static void *navigation_ctrl(void *targs) {
  DroneData *drone = targs;
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

  return NULL;
}

static void *fuel_ctrl(void *targs) { return NULL; }

static void *weapon_ctrl(void *targs) { return NULL; }

static void *camera_ctrl(void *targs) { return NULL; }

static void *network_ctrl(void *targs) {
  DroneData *drone = targs;
  int command_fd;

  print_log(drone->namespace, "Connecting to command");
  if ((command_fd = connect_to_server(SERVER_PORT)) == -1) {
    print_error(drone->namespace, "Can't connect to command");
    exit(1);
  }

  close(command_fd);

  return NULL;
}

void drone(Drone *drone_data, int truck_port) {
  DroneData drone;
  pthread_t systems[N_SYSTEMS];
  int i;

  drone.truck_port = truck_port;
  drone.id = drone_data->id;
  asprintf(&drone.namespace, "%s:%lu:%s", DRONE, drone_data->id,
           drone_data->type == ATTACK ? "ATTACK" : "CAMERA");

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
