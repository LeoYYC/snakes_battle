#ifndef DISPLAY_H__
#define DISPLAY_H__

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#include "snakes.h"
#include "utils/config.h"

int start();
int init_game();
void get_input();
void show(); // void render(); // ��Ⱦ��������
void show_information(); // ��ʾ��Ϸ��Ϣ��ǰ���У�
void show_foods(); // ��ʾʳ��
void show_snakes(); // ��ʾ�ߣ���ȡ�ߵĽڵ�
void point_to_line(node* head); // ����ͷ������Ľڵ㡢��β��������
void blank_snake();
void blank_line(node* head);

void setup_aio_buffer(); // �첽IO

void snake_move();

void game_over(int sig);

int set_ticker(int n_msecs);

#endif // DISPLAY_H__
