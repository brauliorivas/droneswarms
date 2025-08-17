#include "command.h"
#include "constants.h"
#include "enemy.h"
#include "message.h"
#include "physics.h"
#include "random.h"
#include "swarm.h"
#include "truck.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define DEFENSES 2
#define MAIN "MAIN"

int main(int argc, char *argv[]) {
  char *message;
  if (argc != 2) {
    asprintf(&message, "Run: %s <path/to/constants/file>", argv[0]);
    print_error(MAIN, message);
    free(message);
    exit(1);
  }

  srand(time(NULL));

  Constants constants;
  parse_constans(argv[1], &constants);

  int ntrucks = constants.x + 1;
  int ntargets = constants.x;

  pid_t pid;
  int i, j;

  Position *enemy_pos;
  for (i = 0; i < DEFENSES; i++) {
    pid = fork();
    if (pid == -1) {
      print_error(MAIN, "Error forking enemy");
      exit(1);
    } else if (pid == 0) {
      enemy_pos = malloc(sizeof(*enemy_pos));
      if (i == 0) {
        enemy_pos->y = MAX_Y;
        enemy_pos->x = MIN_X;
      } else if (i == 1) {
        enemy_pos->y = MAX_Y;
        enemy_pos->x = MAX_X;
      }
      enemy();
      print_error(MAIN, "Enemy should have finished");
    }
  }

  val truck_step = MAX_X / ntrucks;
  Range truck_location = {MIN_X, MIN_X + truck_step - 1};

  Target *targets;
  targets = malloc(ntargets * sizeof(*targets));

  val target_step = MAX_X / ntargets;
  Range target_location = {MIN_X, MIN_X + target_step - 1};
  for (i = 0; i < ntargets; i++) {
    targets[i].pos.y = MAX_Y;
    targets[i].pos.x = random_value(&target_location);
    targets[i].id = i;
    target_location.x1 = target_location.x2 + 1;
    target_location.x2 = target_location.x2 + target_step;
  }

  int n_attack_drones = N_ATTACK_DRONES * ntargets;
  int *attack_targets = malloc(n_attack_drones * sizeof(*attack_targets));
  for (i = 0; i < n_attack_drones; i++) {
    attack_targets[i] = i;
    printf("drone id: %d\n", attack_targets[i]);
  }
  shuffle(attack_targets, n_attack_drones);

  int n_camera_drones = N_CAMERA_DRONES * ntargets;
  int *camera_targets = malloc(n_camera_drones * sizeof(*camera_targets));
  for (i = 0; i < n_camera_drones; i++) {
    camera_targets[i] = i;
    printf("drone id: %d\n", camera_targets[i]);
  }
  shuffle(camera_targets, n_camera_drones);

  Truck *truck_data;
  pid = fork();
  if (pid == -1) {
    print_error(MAIN, "Error forking truck for command");
    exit(1);
  } else if (pid == 0) {
    print_log(MAIN, "truck has started its execution");
    truck_data = malloc(sizeof(*truck_data));
    truck_data->position.y = MIN_Y;
    truck_data->position.x = random_value(&truck_location);
    truck_data->id = -1;
    command_truck(targets, attack_targets, camera_targets, ntargets);
    print_error(MAIN, "Command truck should have finished");
    exit(1);
  }

  sleep(3);

  for (i = 0; i < (ntrucks - 1); i++) {
    pid = fork();
    if (pid == -1) {
      print_error(MAIN, "Error forking truck");
      exit(1);
    } else if (pid == 0) {
      print_log(MAIN, "truck has started its execution");
      truck_data = malloc(sizeof(*truck_data));
      truck_data->position.y = MIN_Y;
      truck_data->position.x = random_value(&truck_location);
      truck_data->id = i;
      for (j = 0; j < N_ATTACK_DRONES; j++) {
        truck_data->targets[j] =
            &targets[attack_targets[i * N_ATTACK_DRONES + j] % ntargets];
      }
      truck_data->targets[j] = &targets[camera_targets[i]];
      free(attack_targets);
      free(camera_targets);
      drone_truck(truck_data);
      print_error(MAIN, "Truck should have finished");
      exit(1);
    }
  }

  print_log(MAIN, "The simulation should have started, goodbye!...");

  free(targets);
  free(attack_targets);
  free(camera_targets);

  return 0;
}
