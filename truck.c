#include "truck.h"
#include "command.h"
#include "drone.h"
#include "message.h"
#include "network.h"
#include "physics.h"
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
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
    command(targets, attack_targets, camera_targets, ntargets);
  }

  print_log("TRUCK:COMMAND", "Truck sen't all its data to command, done");
  exit(0);
}

void drone_truck(Truck *truck) {
  int i, port, listen_fd, drone_fd;
  pid_t drone_pid;
  Drone *drone_data;
  char *namespace;
  struct sockaddr_in client_addr;
  unsigned int client_len;

  port = SERVER_PORT + truck->id + 1;
  asprintf(&namespace, "%s:%lu:%d", TRUCK, truck->id, port);

  print_log(namespace, "Truck has started its execution");

  if ((listen_fd = create_server_socket(port)) == -1) {
    print_error(namespace, "Can't create truck server");
    exit(1);
  }

  for (i = 0; i < DRONES_PER_TRUCK; i++) {
    drone_pid = fork();
    if (drone_pid == -1) {
      print_error(namespace, "There was an error creating drone");
      exit(1);
    } else if (drone_pid == 0) {
      close(listen_fd);

      drone_data = malloc(sizeof(*drone_data));
      drone_data->id = truck->id * DRONES_PER_TRUCK + i;
      drone_data->type = i < CAMERA_DRONE_IDX ? ATTACK : CAMERA;

      drone(drone_data, port);

      print_error(namespace, "Drone should have finished");
      exit(1);
    } else {
      if ((drone_fd = accept(listen_fd, (struct sockaddr *)&client_addr,
                             &client_len)) == -1) {
        print_error(namespace,
                    "Truck wasn't able to connect to corresponding drone");
        exit(1);
      }

      if (write(drone_fd, &truck->position, sizeof(Position)) == -1) {
        print_error(namespace, "Can't send original position to drone");
        exit(1);
      } // send truck location

      if (write(drone_fd, truck->targets[i], sizeof(Target)) == -1) {
        print_error(namespace, "Can't send target to drone");
        exit(1);
      } // send target

      print_log(namespace, "Truck sent its data to drone");

      close(drone_fd);
    }
  }

  print_log(namespace, "Truck has finished its job");

  close(listen_fd);
  free(namespace);

  exit(0);
}
