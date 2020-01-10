#include "client.h"
#include <stdio.h>

int server_socket_fd;

int connect_to(char* host, int portnum)
{
	server_socket_fd = connect_to_server(host, portnum);
	return 0;
}

int join_room(int roomid)
{
	char* request = "join_room: 1";
	FILE* fd = fdopen(server_socket_fd, "w");
	fputs(request, fd);
	fclose(fd);

	return 0;
}

int add_player(char* nname)
{
	return 0;
}
