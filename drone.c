#include "network.h"
#include <stdlib.h>

void drone() {
  int server_fd = connect_to_server(SERVER_PORT);

  exit(0);
}
