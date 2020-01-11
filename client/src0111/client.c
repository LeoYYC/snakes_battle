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
	//���ȹ���һ��������ָ�룺
	cJSON* root = NULL;
	//����int����ָ�룺
	cJSON* int_number = NULL;
	//����һ�������󣬸�root
	root = cJSON_CreateObject();
	//����һ��int���󣬸�int_number
	int_number = cJSON_CreateNumber(roomid);
	//���ַ���string��������󣬳�Ϊһ����ֵ�ԣ�
	cJSON_AddItemToObject(root, "join_room", int_number);

	// �任
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
	//���ȹ���һ��������ָ�룺
	cJSON* root = NULL;
	//����һ�������󣬸�root
	root = cJSON_CreateObject();
	//����String����ָ�룺
	cJSON* string = NULL;
	//����һ���ַ������󣬸�string
	string = cJSON_CreateString(nname);
	//���ַ���string��������󣬳�Ϊһ����ֵ�ԣ�
	cJSON_AddItemToObject(root, "player_name", string);
	//cJSON_AddStringToObject(root, "player_name", nname);

	// �任
	request = cJSON_PrintUnformatted(root);

	//FILE* fd = fdopen(server_socket_fd, "w");
	//fputs(request, fd);
	//fclose(fd);

	sendto(server_socket_fd, request, 50, 0, NULL, NULL);

	cJSON_Delete(root);

	return 0;
}
