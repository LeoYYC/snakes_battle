#ifndef MANAGE_H__
#define MANAGE_H__

#include "config.h"

struct client_info 
{
	char* nname; // player nick name
	int playerid;
	struct sockaddr* clt_sockaddr; // client sock address
	int roomid;
};

int add_player();
int expel_player();

int get_player_sum();
int add_player_sum();
int minus_player_sum();


#endif // MANAGE_H__
