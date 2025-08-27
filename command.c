#include "command.h"
#include "message.h"
#include "network.h"
#include "queue.h"
#include "swarm.h"
#include "truck.h"
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define COMMAND "COMMAND"

typedef struct {
  struct swarm_head *swarm_list;
  int *drone_fd;
} Command;

static void *attend_drone(void *targs) {
  Command *command_data;
  int *drone_fd;

  command_data = targs;
  drone_fd = command_data->drone_fd;

  close(*drone_fd);
  free(drone_fd);
  free(command_data);
  return NULL;
}

void command(Target *targets, int *attack_targets, int *camera_targets,
             int ntargets) {
  unsigned int drone_len;
  struct sockaddr_in drone_addr;
  int i, j, k, listen_fd, nswarms;
  Swarm *swarm;
  Drone *drone;
  struct swarm_head swarm_list;
  Command *command_data;

  nswarms = ntargets;

  LIST_INIT(&swarm_list);

  for (i = 0; i < nswarms; i++) {
    swarm = malloc(sizeof(*swarm));
    LIST_INSERT_HEAD(&swarm_list, swarm, swarm_l);
    swarm->id = i;
    swarm->target = &targets[i];
    LIST_INIT(&swarm->drone_list);
    for (j = 0; j < N_ATTACK_DRONES; j++) {
      drone = malloc(sizeof(*drone));
      LIST_INSERT_HEAD(&swarm->drone_list, drone, drone_l);
      drone->type = ATTACK;
    }
    for (j = 0; j < N_CAMERA_DRONES; j++) {
      drone = malloc(sizeof(*drone));
      LIST_INSERT_HEAD(&swarm->drone_list, drone, drone_l);
      drone->type = CAMERA;
    }
    for (j = 0; j < N_CAMERA_DRONES * ntargets; j++) {
      if (camera_targets[j] % ntargets == i) {
        drone->id = (j + 1) * DRONES_PER_TRUCK - 1;
        drone = drone->drone_l.le_next;
      }
    }
    for (j = 0; j < N_ATTACK_DRONES * ntargets; j++) {
      if (attack_targets[j] % ntargets == i) {
        drone->id = j + (j / N_ATTACK_DRONES);
        drone = drone->drone_l.le_next;
      }
    }
    LIST_FOREACH(drone, &swarm->drone_list, drone_l) {
      vprint_log(COMMAND, "swarm: %d drone id: %lu drone type: %s", swarm->id,
                 drone->id, drone->type == ATTACK ? "ATTACK" : "CAMERA");
    }
  }

  listen_fd = create_server_socket(SERVER_PORT);

  int n_drones = ntargets * DRONES_PER_TRUCK;
  pthread_t *workers = malloc(n_drones * sizeof(*workers));

  for (i = 0; i < n_drones; i++) {
    command_data = malloc(sizeof(*command_data));
    command_data->swarm_list = &swarm_list;
    command_data->drone_fd = malloc(sizeof(*command_data->drone_fd));
    *command_data->drone_fd =
        accept(listen_fd, (struct sockaddr *)&drone_addr, &drone_len);
    if (pthread_create(&workers[i], NULL, attend_drone, command_data) != 0) {
      print_error(COMMAND, "Could not connect to drone");
    }
  }

  close(listen_fd);
  free(workers);

  exit(0);
}
