#ifndef SNAKES_H__
#define SNAKES_H__

#include "config.h"

struct snake_node 
{
	char shape; // @ is the head, # is the body
	int xloc;
	int yloc;
};

struct coordinate 
{
	int xloc;
	int yloc;
};

typedef struct node
{
	struct node* pre;
	char shape; // @ is the head, # is the body
	//struct coordinate loc;
	int xloc;
	int yloc;
	struct node* next;
}node;

struct player 
{
	int socketid;
	int playerid;
};

struct player_info
{
	char* nname; // nick name
	int playerid;
	struct sockaddr* clt_sockaddr; // client sockaddr
	int roomid;
};

struct snake 
{
	int playerid;
	struct node* snake_head;
	struct node* snake_tail;
};

int get_my_snake_length();
struct coordinate* get_foods_loc();
node* get_me_snake();
node* get_me_snake_tail();
node** get_other_snakes();

void insert_node(int x, int y, node* head);
void delete_node(node* tail);
void delete_link(node* head, node* tail);

void upload_me_snake_data();
void download_other_snakes_data();
void download_me_status();

void init_me_snake(int COLS, int LINES);

#endif // SNAKES_H__
