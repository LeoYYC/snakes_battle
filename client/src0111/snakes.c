#include "snakes.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "utils/cJSON.h"

int snake_length = 1;
struct coordinate foods_loc[FOODS_NUM];
struct player me_player;

//struct snake_node me_snake[MAX_SNAKE_JOINTS];
//struct snake_node all_other_snakes[MAX_PLAYERS_PER_ROOM - 1][MAX_SNAKE_JOINTS];

node* me_head, * me_tail;

node* all_other_snakes[MAX_PLAYERS_PER_ROOM - 1][2];

int get_my_snake_length()
{
	return snake_length;
}

struct coordinate* get_foods_loc()
{
	return foods_loc;
}

node* get_me_snake()
{
	return me_head;
}

node* get_me_snake_tail()
{
	return me_tail;
}

node** get_other_snakes()
{
	return all_other_snakes;
}

void init_me_snake(int COLS, int LINES)
{
	me_head = (node*)malloc(sizeof(node));
	me_tail = (node*)malloc(sizeof(node));
	creat_link(me_head, me_tail);
	insert_node(5, 15, me_head);
	insert_node(10, 15, me_head);
	insert_node(10, 20, me_head);
}

void set_me_player(char* nname, int playerid, int socketid)
{
	strncpy(me_player.nname, nname, 20);
	me_player.playerid = playerid;
	me_player.socketid = socketid;
}

void upload_me_snake_data(int server_socket_fd)
{
	char* request = { 0 };
	//���ȹ���һ��������ָ�룺
	cJSON* root = NULL;
	//����String����ָ�룺
	cJSON* string = NULL;
	//����һ�������󣬸�root
	root = cJSON_CreateObject();
	//����һ���ַ������󣬸�string
	string = cJSON_CreateString(me_player.nname);
	cJSON* playerid = cJSON_CreateNumber(me_player.playerid);
	cJSON* snake_array = cJSON_CreateArray();

	node* temp = me_tail->pre;
	while (temp != me_head)
	{
		int snake_joint[2] = {temp->xloc, temp->yloc};
		cJSON* array = cJSON_CreateIntArray(snake_joint, 2);
		cJSON_AddItemToArray(snake_array, array);
		temp = temp->pre;
	}

    cJSON_AddItemToObject(root, "player_name", string);
	cJSON_AddItemToObject(root, "playerid", playerid);
	cJSON_AddItemToObject(root, "me_snake", snake_array);
	// �任
	request = cJSON_PrintUnformatted(root);

	//FILE* fd = fdopen(server_socket_fd, "w");
	//fputs(request, fd);
	//fclose(fd);

	sendto(server_socket_fd, request, strlen(request), 0, NULL, NULL);

	//write(server_socket_fd, request, sizeof(request));

	cJSON_Delete(root);
}

void download_foods_loc(int server_socket_fd)
{

}

// ����һ����ͷβ����˫������
void creat_link(node* head, node* tail)
{
	node* temp = (node*)malloc(sizeof(node));
	//head = (node*)malloc(sizeof(node));
	//tail = (node*)malloc(sizeof(node));
	temp->xloc = 5;
	temp->yloc = 10;
	temp->shape = SNAKE_HEAD;
	head->pre = tail->next = NULL;
	head->next = temp;
	temp->next = tail;
	tail->pre = temp;
	temp->pre = head;
}

// �������ͷ������ͷ��㣩����һ�����
void insert_node(int x, int y, node* head)
{
	node* temp = (node*)malloc(sizeof(node));
	temp->xloc = x;
	temp->yloc = y;
	temp->shape = SNAKE_HEAD;
	temp->next = head->next;
	head->next = temp;
	temp->pre = head;
	temp->next->pre = temp;
}

// ɾ������ģ���β���ģ����һ�����
void delete_node(node* tail)
{
	node* temp = tail->pre;
	node* bTemp = temp->pre;
	bTemp->next = tail;
	tail->pre = bTemp;
	temp->next = temp->pre = NULL;
	free(temp);
	temp = NULL;
}

// ɾ����������
void delete_link(node* head, node* tail)
{
	while (head->next != tail)
		delete_node(tail);
	head->next = tail->pre = NULL;
	free(head);
	free(tail);
}