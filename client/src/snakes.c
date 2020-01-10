#include "snakes.h"
#include <stdio.h>

int snake_length = 1;
struct coordinate foods_loc[FOODS_NUM];

//struct snake_node me_snake[MAX_SNAKE_JOINTS];
//struct snake_node all_other_snakes[MAX_PLAYERS_PER_ROOM - 1][MAX_SNAKE_JOINTS];

node* head, * tail;

node* all_other_snakes_head[MAX_PLAYERS_PER_ROOM - 1];

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
	return head;
}

node* get_me_snake_tail()
{
	return tail;
}

node** get_other_snakes()
{
	return all_other_snakes_head;
}

void init_me_snake(int COLS, int LINES)
{
	creat_link();
	insert_node(5, 15, head);
	insert_node(10, 15, head);
	insert_node(10, 20, head);
	srand(time(0));
	//----------tmp
	int i;
	for (i = 0; i < FOODS_NUM; i++)
	{
		foods_loc[i].yloc = rand() % COLS;
		foods_loc[i].xloc = rand() % (LINES - 2) + 2;
	}
	//----------tmp  
}

// ����һ����ͷβ����˫������
void creat_link()
{
	node* temp = (node*)malloc(sizeof(node));
	head = (node*)malloc(sizeof(node));
	tail = (node*)malloc(sizeof(node));
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