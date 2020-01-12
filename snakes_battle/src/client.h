#ifndef CLIENT_H__
#define CLIENT_H__
#include <stdio.h>
#include "utils/clientlib.h"

int connect_to(char* host, int portnum);

void* get_data(void* arg);

int join_room(int roomid);
int add_player(char* nname);

int get_server_socket_fd();

#endif // CLIENT_H__
