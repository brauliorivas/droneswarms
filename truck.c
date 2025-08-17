#include "truck.h"
#include "command.h"
#include "drone.h"
#include "message.h"
#include "physics.h"
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#define TRUCK "TRUCK"

void command_truck(Target *targets, int *attack_targets, int *camera_targets,
                   int ntargets) {
  pid_t child;

  child = fork();
  if (child == -1) {
    print_error(TRUCK, "There was an error creating command");
    exit(1);
  } else if (child == 0) {
    print_log(TRUCK, "command has started its execution");
    command(targets, attack_targets, camera_targets, ntargets);
  }
}

void drone_truck(Truck *truck) {
  int i;
  pid_t drone_pid;

  for (i = 0; i < DRONES_PER_TRUCK; i++) {
    drone_pid = fork();
    if (drone_pid == -1) {
      print_error(TRUCK, "There was an error creating drone");
      exit(1);
    } else if (drone_pid == 0) {
      print_log(TRUCK, "drone has started its execution");
      drone();
    }
  }

  exit(0);
}
