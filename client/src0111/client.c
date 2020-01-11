#include "client.h"

#include "utils/cJSON.h"
#include "snakes.h"

int server_socket_fd;
//FILE* fd;

int connect_to(char* host, int portnum)
{
	server_socket_fd = connect_to_server(host, portnum);
	//fd = fdopen(server_socket_fd, "w");
	return 0;
}

int close_to()
{
	//close(fd);
	close(server_socket_fd);

	return 0;
}

int get_server_socket_fd()
{
	return server_socket_fd;
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
	
	//FILE* fd = fdopen(server_socket_fd, "w");
	//fputs(request, fd);
	//fclose(fd);

	sendto(server_socket_fd, request, 50, 0, NULL, NULL);

	//write(server_socket_fd, request, sizeof(request));

	cJSON_Delete(root);

	return 0;
}

int add_player(char* nname)
{
	set_me_player(nname, 0, 0);

	char* request = { 0 };
	//首先构造一个根对象指针：
	cJSON* root = NULL;
	//创建一个根对象，给root
	root = cJSON_CreateObject();
	//创建String对象指针：
	cJSON* string = NULL;
	//创建一个字符串对象，给string
	string = cJSON_CreateString(nname);
	//将字符串string加入根对象，成为一个键值对：
	cJSON_AddItemToObject(root, "player_name", string);
	//cJSON_AddStringToObject(root, "player_name", nname);

	// 变换
	request = cJSON_PrintUnformatted(root);

	//FILE* fd = fdopen(server_socket_fd, "w");
	//fputs(request, fd);
	//fclose(fd);

	sendto(server_socket_fd, request, 50, 0, NULL, NULL);

	cJSON_Delete(root);

	return 0;
}
