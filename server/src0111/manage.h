#ifndef MANAGE_H__
#define MANAGE_H__

#include "utils/config.h"

struct client_info 
{
	char* nname; // player nick name
	int playerid;
	struct sockaddr* clt_sockaddr; // client sock address
	int roomid;
};

struct coordinate
{
	int xloc;
	int yloc;
};

struct player
{
	int socketid;
	int playerid;
	char nname[20];
};

int add_player(char* request, int socketfdid);

int expel_player();
int get_player_sum();
int add_player_sum();
int minus_player_sum();

void update_foods(int i);
int gather(char* request);
int convey();

int detect();


#endif // MANAGE_H__
