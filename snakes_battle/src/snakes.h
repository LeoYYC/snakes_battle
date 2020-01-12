#ifndef SNAKES_H__
#define SNAKES_H__

#include "utils/config.h"

#include "utils/cJSON.h"

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
	char nname[20];
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


void upload_me_snake_data(int server_socket_fd);
void handle_snakes_data(char* snakes_data_json);
void handle_me_status(char* me_status);
void handle_foods_loc(char* foods_loc_json);

void set_me_player(char* nname, int playerid, int socketid);

void init_me_snake(int COLS, int LINES);

void creat_link(node* head, node* tail, int xl, int yl);
void insert_node(int x, int y, node* head);
void delete_node(node* tail);
void delete_link(node* head, node* tail);

#endif // SNAKES_H__
