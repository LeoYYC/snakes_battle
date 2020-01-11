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

	FILE* fd = fdopen(server_socket_fd, "w");
	fputs(request, fd);
	fclose(fd);

	return 0;
}

int add_player(char* nname)
{
	return 0;
}
