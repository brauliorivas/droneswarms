#ifndef __NETWORK_H__
#define __NETWORK_H__

#define SERVER_PORT 3000
#define LOCALHOST "127.0.0.1"

#define READ_IDX 0
#define WRITE_IDX 1

int create_server_socket(int port);
int connect_to_server(int port);

#endif 
