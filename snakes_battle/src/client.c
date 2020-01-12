#include "client.h"
#include "snakes.h"

#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

#include "utils/msg.h"

int server_socket_fd;

pthread_t get_data_tid;

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

void* get_data(void* arg)
{
	int sockfdid = *(int*)arg;
	char buf[BUFSIZ];
	int n;

	//one_msg(MSG_INFO, "get_data thread in");

	while ((n = read(sockfdid, buf, BUFSIZ)) > 0)
	{
		//printf("[%s]: %s\n", MSG_IN_DATA, buf);

		cJSON* json = cJSON_Parse(buf);
		if (strcmp(cJSON_GetObjectItem(json, "cmd")->valuestring, "players_data") == 0)
		{
			handle_snakes_data(buf);
		}
		else if (strcmp(cJSON_GetObjectItem(json, "cmd")->valuestring, "foods_loc") == 0)
		{
			handle_foods_loc(buf);
		}
		else if (strcmp(cJSON_GetObjectItem(json, "cmd")->valuestring, "me_status") == 0)
		{
		}
		else
		{
			one_msg(MSG_INFO, "json data cannot match");
		}
		
		bzero(buf, BUFSIZ);
		cJSON_Delete(json);
	}
}

int get_server_socket_fd()
{
	return server_socket_fd;
}

int join_room(int roomid)
{
	char* request = { 0 };
	cJSON* root = cJSON_CreateObject();
	cJSON* int_number = cJSON_CreateNumber(roomid);
	cJSON_AddStringToObject(root, "cmd", "join_room");
	cJSON_AddItemToObject(root, "room_num", int_number);

	request = cJSON_PrintUnformatted(root);

	sendto(server_socket_fd, request, 50, 0, NULL, NULL);

	cJSON_Delete(root);
	free(request);

	return 0;
}

int add_player(char* nname)
{
	//set_me_player(nname, 0, 0);

	char* request = { 0 };
	cJSON* root = NULL;
	root = cJSON_CreateObject();
	cJSON* string  = cJSON_CreateString(nname);
	cJSON_AddStringToObject(root, "cmd", "add_player");
	cJSON_AddItemToObject(root, "player_name", string);

	request = cJSON_PrintUnformatted(root);

	sendto(server_socket_fd, request, strlen(request), 0, NULL, NULL);

	cJSON_Delete(root);
	free(request);

	char response[RESPONSE_LEN];
	recvfrom(server_socket_fd, response, RESPONSE_LEN, 0, NULL, NULL);
	//one_msg(MSG_IN_DATA, response);

	cJSON* json = cJSON_Parse(response);
	if (strcmp(cJSON_GetObjectItem(json, "cmd")->valuestring, "add_player_response") != 0)
	{
		return 1;
	}
	set_me_player(cJSON_GetObjectItem(json, "player_nname")->valuestring, cJSON_GetObjectItem(json, "player_id")->valueint, server_socket_fd);

	cJSON_Delete(json);

	pthread_create(&get_data_tid, NULL, (void*)get_data, (void*)&server_socket_fd);

	return 0;
}


