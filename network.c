#include "network.h"
#include "message.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define NETWORK "NETWORK"

int create_server_socket(int port) {
  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd == -1) {
    print_error(NETWORK, "Error running socket()");
    return -1;
  }
  int yes = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) ==
      -1) {
    print_error(NETWORK, "Error runnign socketopt()");
    close(listen_fd);
    return -1;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(port);
  if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    print_error(NETWORK, "Error running bind()");
    close(listen_fd);
    return -1;
  }
  if (listen(listen_fd, 1) == -1) {
    print_error(NETWORK, "Error running listen()");
    close(listen_fd);
    return -1;
  }
  return listen_fd;
}

int connect_to_server(int port) {
  char *message;
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    print_error(NETWORK, "Cant create connect socket");
    return -1;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons((uint16_t)port);
  addr.sin_addr.s_addr = inet_addr(LOCALHOST);

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    print_error(NETWORK, "Can't connect to server");
    close(fd);
    return -1;
  }

  asprintf(&message, "Connected to %s:%d", LOCALHOST, port);
  print_log(NETWORK, message);
  free(message);

  return fd;
}
