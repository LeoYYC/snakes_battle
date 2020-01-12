#include "manage.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils/cJSON.h"
#include "utils/msg.h"
#include <string.h>

#define PLAYER_LOC_LEN 300

//struct client_info all_players[PREFORK][MAX_PLAYERS_PER_ROOM];
struct coordinate foods_loc[FOODS_NUM];

char player_data[MAX_PLAYERS_PER_ROOM][PLAYER_LOC_LEN];

struct player players[MAX_PLAYERS_PER_ROOM];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void init_data()
{
	int i;
	for (i = 0; i < MAX_PLAYERS_PER_ROOM; i++)
	{
		players[i].playerid = -1;
	}

	srand(time(0));
	for (i = 0; i < FOODS_NUM; i++)
	{
		foods_loc[i].yloc = rand() % COLS;
		foods_loc[i].xloc = rand() % (LINES - 2) + 2;
	}
}

void update_foods(int i)
{
	foods_loc[i].yloc = rand() % COLS;
	foods_loc[i].xloc = rand() % (LINES - 2) + 2;
}

void* convey_foods()
{
	char* foods_data = { 0 };
	cJSON* root = cJSON_CreateObject();
	cJSON* foods_data_array = cJSON_CreateArray();
	//cJSON* cmd = cJSON_CreateString("foods_loc");
	int i;
	for (i = 0; i < FOODS_NUM; i++)
	{
		cJSON* food_loc = cJSON_CreateObject();
		cJSON* fxloc = cJSON_CreateNumber(foods_loc[i].xloc);
		cJSON* fyloc = cJSON_CreateNumber(foods_loc[i].yloc);
		cJSON_AddItemToObject(food_loc, "xloc", fxloc);
		cJSON_AddItemToObject(food_loc, "yloc", fyloc);
		cJSON_AddItemToArray(foods_data_array, food_loc);
	}
	cJSON_AddStringToObject(root, "cmd", "foods_loc");
	cJSON_AddItemToObject(root, "foods_data", foods_data_array);

	foods_data = cJSON_PrintUnformatted(root);
	
	//pthread_t tid;
	//pthread_create(&tid, NULL, (void*)thread_convey, foods_data);
	
	for (i = 0; i < MAX_PLAYERS_PER_ROOM; i++)
	{
		if (players[i].playerid == -1)
		{
			continue;
		}
		else
		{
			sendto(players[i].socketid, foods_data, strlen(foods_data), 0, NULL, NULL);
		}
	}

	cJSON_Delete(root);
	free(foods_data);
}

int add_player(char* request, int socketfdid)
{
	one_msg(MSG_INFO, "add_player is called");
	cJSON* json = cJSON_Parse(request);
	cJSON* nname = cJSON_GetObjectItem(json, "player_name");
	int i;
	for (i = 0; i < MAX_PLAYERS_PER_ROOM; i++)
	{
		if (players[i].playerid == -1)
		{
			players[i].playerid = i;
			strcpy(players[i].nname, nname->valuestring); //, strlen(nname->valuestring) + 1);
			players[i].socketid = socketfdid;
			break;
		}
	}
	cJSON_Delete(json);

	char* response = { 0 };
	cJSON* root =  cJSON_CreateObject();
	cJSON* player_nname = cJSON_CreateString(players[i].nname);
	cJSON* player_id = cJSON_CreateNumber(i);
	cJSON_AddStringToObject(root, "cmd", "add_player_response");
	cJSON_AddItemToObject(root, "player_nname", player_nname);
	cJSON_AddItemToObject(root, "player_id", player_id);

	response = cJSON_PrintUnformatted(root);

	one_msg(MSG_OUT_DATA, response);

	sendto(socketfdid, response, strlen(response), 0, NULL, NULL);

	cJSON_Delete(root);
	free(response);

	return i;
}

int gather(char* request)
{
	one_msg(MSG_INFO, "gather is called");

	cJSON* json = cJSON_Parse(request);
	cJSON* playerid = cJSON_GetObjectItem(json, "playerid");
	cJSON* me_snake = cJSON_GetObjectItem(json, "me_snake");

	char* me_snake_data = { 0 };
	// cJSON_GetObjectItem(json, "me_snake")是一个cJSON的数组对象，不能直接传入到strcpy
	me_snake_data = cJSON_PrintUnformatted(me_snake);
	
	pthread_mutex_lock(&mutex);
	if (playerid->valueint < MAX_PLAYERS_PER_ROOM && playerid->valueint >= 0)
	{
		strncpy(player_data[playerid->valueint], me_snake_data, strlen(me_snake_data) + 1);
	}
	pthread_mutex_unlock(&mutex);

	cJSON_Delete(json);
	free(me_snake_data);

	return 0;
}

//void* thread_convey(void* arg) {
//	char* players_data = (char*)arg;
//	int i;
//	for (i = 0; i < MAX_PLAYERS_PER_ROOM; i++)
//	{
//		if (players[i].playerid == -1)
//		{
//			continue;
//		}
//		else
//		{
//			sendto(players[i].socketid, players_data, strlen(players_data), 0, NULL, NULL);
//		}
//	}
//
//	return NULL;
//}

void* convey(void* arg)
{
	one_msg(MSG_INFO, "convey thread in");

	char* players_data = { 0 };
	cJSON* root = cJSON_CreateObject();
	cJSON* players_data_json = cJSON_CreateObject();

	int i;
	char playerid[3];

	char cjsonplayerid[10];
	for (i = 0; i < MAX_PLAYERS_PER_ROOM; i++)
	{
		if (player_data[i][0] == '\0')
		{
			continue;
		}
		else
		{
			//cJSON* string = cJSON_CreateString(player_data[i]);
			sprintf(playerid, "%d", i);
			//cJSON_AddItemToObject(players_data_json, playerid, string);
			cJSON* string_ogj = cJSON_Parse(player_data[i]);
			cJSON_AddItemToObject(players_data_json, playerid, string_ogj);
		}
	}

	cJSON_AddStringToObject(root, "cmd", "players_data");
	cJSON_AddItemToObject(root, "players_data", players_data_json);
	// 变换
	players_data = cJSON_PrintUnformatted(root);

	one_msg(MSG_OUT_DATA, players_data);

	//pthread_t tid;
	//pthread_create(&tid, NULL, (void*)thread_convey, players_data);

	for (i = 0; i < MAX_PLAYERS_PER_ROOM; i++)
	{
		if (players[i].playerid == -1)
		{
			continue;
		}
		else
		{
			sendto(players[i].socketid, players_data, strlen(players_data), 0, NULL, NULL);
		}
	}

	cJSON_Delete(root);
	free(players_data);
}
