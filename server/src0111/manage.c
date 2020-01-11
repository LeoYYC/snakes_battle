#include "manage.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils/cJSON.h"

#define PLAYER_LOC_LEN 300

//struct client_info all_players[PREFORK][MAX_PLAYERS_PER_ROOM];
struct coordinate foods_loc[FOODS_NUM];

char player_data[MAX_PLAYERS_PER_ROOM][PLAYER_LOC_LEN];

struct player players[MAX_PLAYERS_PER_ROOM];

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


int add_player(char* request, int socketfdid)
{
	cJSON* json = cJSON_Parse(request);
	cJSON* nname = cJSON_GetObjectItem(json, "nname");

	int i;
	for (i = 0; i < MAX_PLAYERS_PER_ROOM; i++)
	{
		if (players[i].playerid == -1)
		{
			players[i].playerid = i;
			strncpy(players[i].nname, nname->valuestring, strlen(nname->valuestring) + 1);
			players[i].socketid = socketfdid;
			break;
		}
	}

	cJSON_Delete(json);

	return i;
}

int gather(char* request)
{
	cJSON* json = cJSON_Parse(request);
	cJSON* playerid = cJSON_GetObjectItem(json, "playerid");

	if (playerid->valueint < MAX_PLAYERS_PER_ROOM && playerid->valueint >= 0)
	{
		strncpy(player_data[playerid->valueint], request, strlen(request) + 1);
	}

	cJSON_Delete(json);

	return 0;
}

void* thread_convey(void* arg) {
	char* players_data = (char*)arg;
	int i;
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

	return NULL;
}

int convey()
{
	pthread_t tid;

	char* players_data = { 0 };
	cJSON* root = cJSON_CreateObject();
	cJSON* players_data_json = cJSON_CreateObject();

	int i;
	char playerid[3];
	for (i = 0; i < MAX_PLAYERS_PER_ROOM; i++)
	{
		if (player_data[i][0] == '\0')
		{
			continue;
		}
		else
		{
			cJSON* string = cJSON_CreateString(player_data[i]);
			itoa(i, playerid, 10);
			cJSON_AddItemToObject(players_data_json, playerid, string);
		}
	}

	cJSON_AddItemToObject(root, "players_data", players_data_json);
	// ±ä»»
	players_data = cJSON_PrintUnformatted(root);

	pthread_create(&tid, NULL, (void*)thread_convey, players_data);

	cJSON_Delete(root);
}
