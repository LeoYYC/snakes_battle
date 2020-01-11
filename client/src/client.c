#include "client.h"

#include "utils/cJSON.h"

int server_socket_fd;

int connect_to(char* host, int portnum)
{
	server_socket_fd = connect_to_server(host, portnum);
	return 0;
}

int join_room(int roomid)
{
	char* request = { 0 };
	//首先构造一个根对象指针：
	cJSON* root = NULL;
	//创建int对象指针：
	cJSON* int_number = NULL;
	//创建一个根对象，给root
	root = cJSON_CreateObject();
	//创建一个int对象，给int_number
	int_number = cJSON_CreateNumber(roomid);
	//将字符串string加入根对象，成为一个键值对：
	cJSON_AddItemToObject(root, "join_room", int_number);

	// 变换
	request = cJSON_PrintUnformatted(root);

	FILE* fd = fdopen(server_socket_fd, "w");
	fputs(request, fd);
	fclose(fd);

	return 0;
}

int add_player(char* nname)
{
	return 0;
}
