#include "snakes.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int snake_length = 1;
struct coordinate foods_loc[FOODS_NUM];
struct player me_player;

//struct snake_node me_snake[MAX_SNAKE_JOINTS];
//struct snake_node all_other_snakes[MAX_PLAYERS_PER_ROOM - 1][MAX_SNAKE_JOINTS];

node* me_head, * me_tail;

//node* all_other_snakes[MAX_PLAYERS_PER_ROOM][2];
node* all_other_snakes[MAX_PLAYERS_PER_ROOM];

pthread_mutex_t handle_snakes_data_mutex = PTHREAD_MUTEX_INITIALIZER;

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
	creat_link(me_head, me_tail, 5, 10);
	insert_node(5, 15, me_head);
	insert_node(10, 15, me_head);
	insert_node(10, 20, me_head);
}

node* create_new_link(int i, int xl, int yl)
{
	node* temp_head = (node*)malloc(sizeof(node));
	node* temp_tail = (node*)malloc(sizeof(node));
	creat_link(temp_head, temp_tail, xl, yl);

	//all_other_snakes[i][0] = temp_head;
	//all_other_snakes[i][1] = temp_tail;

	all_other_snakes[i] = temp_head;

	return temp_head;
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
	cJSON* root = cJSON_CreateObject();
	cJSON* up_data = cJSON_CreateObject();
	cJSON* string = cJSON_CreateString(me_player.nname);
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

	cJSON_AddStringToObject(root, "cmd", "snake_data");
    cJSON_AddItemToObject(root, "player_name", string);
	cJSON_AddItemToObject(root, "playerid", playerid);
	// 下面的语句不能放到上面，未知原因
	cJSON* playerid_copy = cJSON_CreateNumber(me_player.playerid);
	cJSON_AddItemToObject(up_data, "playerid", playerid_copy);
	cJSON_AddItemToObject(up_data, "snake_link", snake_array);
	//cJSON_AddItemToObject(root, "me_snake", snake_array);
	cJSON_AddItemToObject(root, "me_snake", up_data);

	request = cJSON_PrintUnformatted(root);

	sendto(server_socket_fd, request, strlen(request), 0, NULL, NULL);

	cJSON_Delete(root);
	free(request);
}

void handle_foods_loc(char* foods_loc_json)
{

}

void handle_snakes_data(char* snakes_data_json)
{
	cJSON* json = cJSON_Parse(snakes_data_json);
	cJSON* players_data = cJSON_GetObjectItem(json, "players_data");
	
	if (players_data == NULL)
	{
		cJSON_Delete(json);
		return;
	}

	cJSON* players_data_o = players_data->child;

	if (players_data_o == NULL)
	{
		cJSON_Delete(json);
		return;
	}

	cJSON* players_data_temp = players_data_o;

	int link_num;
	while (players_data_temp != NULL)
	{
		cJSON* playerid = cJSON_GetObjectItem(players_data_temp, "playerid");
		cJSON* snake_link = cJSON_GetObjectItem(players_data_temp, "snake_link");
		link_num = cJSON_GetArraySize(snake_link);

		cJSON* snake_link_item = cJSON_GetArrayItem(snake_link, 0);
		
		pthread_mutex_lock(&handle_snakes_data_mutex);
		
		if (all_other_snakes[playerid->valueint] != NULL)
		{
			//delete_link(all_other_snakes[playerid->valueint][0], all_other_snakes[playerid->valueint][1]);
			delete_link_by_head(all_other_snakes[playerid->valueint]);
			create_new_link(playerid->valueint, cJSON_GetArrayItem(snake_link_item, 0)->valueint, cJSON_GetArrayItem(snake_link_item, 1)->valueint);
		}
		else
		{
			create_new_link(playerid->valueint, cJSON_GetArrayItem(snake_link_item, 0)->valueint, cJSON_GetArrayItem(snake_link_item, 1)->valueint);
		}

		int i;
		for (i = 1; i < link_num; i++)
		{
			snake_link_item = cJSON_GetArrayItem(snake_link, i);
			//insert_node(cJSON_GetArrayItem(snake_link_item, 0)->valueint, cJSON_GetArrayItem(snake_link_item, 1)->valueint, all_other_snakes[playerid->valueint][0]);
			insert_node(cJSON_GetArrayItem(snake_link_item, 0)->valueint, cJSON_GetArrayItem(snake_link_item, 1)->valueint, all_other_snakes[playerid->valueint]);
		}

		pthread_mutex_unlock(&handle_snakes_data_mutex);
		players_data_temp = players_data_temp->next;
	}
	
	cJSON_Delete(json);
	//cJSON_Delete(players_data_json);
}

void handle_me_status(char* me_status)
{

}

// 创建一个带头尾结点的双向链表
void creat_link(node* head, node* tail, int xl, int yl)
{
	node* temp = (node*)malloc(sizeof(node));
	//head = (node*)malloc(sizeof(node));
	//tail = (node*)malloc(sizeof(node));
	temp->xloc = xl;
	temp->yloc = yl;
	temp->shape = SNAKE_HEAD;
	head->pre = tail->next = NULL;
	head->next = temp;
	temp->next = tail;
	tail->pre = temp;
	temp->pre = head;
}

// 在链表的头部（非头结点）插入一个结点
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

// 删除链表的（非尾结点的）最后一个结点
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

// 删除整个链表
void delete_link(node* head, node* tail)
{
	while (head->next != tail)
		delete_node(tail);
	head->next = tail->pre = NULL;
	free(head);
	free(tail);
}

// 删除整个链表
void delete_link_by_head(node* head)
{
	node* temp = head->next;
	while (temp->next != NULL)
	{
		temp = temp->next;
	}
	while (head->next != temp)
		delete_node(temp);
	head->next = temp->pre = NULL;
	free(head);
	free(temp);
}