#include "command.h"
#include "message.h"
#include "network.h"
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

static void *attend_drone(void *targs) {
  int *connfd = targs;

  while (1) {
    // communicate with drone
  }

  close(*connfd);
  free(connfd);
  return NULL;
}

void command(Target *targets, int *attack_targets, int *camera_targets,
             int ntargets) {
  unsigned int clientlen;
  struct sockaddr_in clientaddr;
  int i, j, k, listen_fd, *connfd, nswarms = ntargets;
  Swarm *swarms;

  swarms = malloc(nswarms * sizeof(*swarms));

  for (i = 0; i < nswarms; i++) {
    swarms[i].target = &targets[i];
    for (j = 0; j < DRONES_PER_TRUCK; j++) {
      swarms[i].drones[j] = malloc(sizeof(Drone));
    }
    for (j = 0; j < N_ATTACK_DRONES; j++) {
      swarms[i].drones[j]->type = ATTACK;
    }
    swarms[i].drones[j]->type = CAMERA;
    k = 0;
    for (j = 0; j < N_ATTACK_DRONES * ntargets; j++) {
      if (attack_targets[j] % ntargets == i) {
        swarms[i].drones[k]->id = j + (j / N_ATTACK_DRONES);
        k++;
      }
    }
    for (j = 0; j < N_CAMERA_DRONES * ntargets; j++) {
      if (camera_targets[j] % ntargets == i) {
        swarms[i].drones[k]->id = (j + 1) * DRONES_PER_TRUCK - 1;
      }
    }
  }

  exit(0);
  /* network tasks */
  listen_fd = create_server_socket(SERVER_PORT);

  int n_drones = ntargets * DRONES_PER_TRUCK;
  pthread_t *workers = malloc(n_drones * sizeof(*workers));

  for (i = 0; i < n_drones; i++) {
    connfd = malloc(sizeof(*connfd));
    *connfd = accept(listen_fd, (struct sockaddr *)&clientaddr, &clientlen);
    if (pthread_create(&workers[i], NULL, attend_drone, connfd) != 0) {
      print_error(COMMAND, "Could not connect to drone");
    }
  }

  close(listen_fd);
  free(workers);

  exit(0);
}
