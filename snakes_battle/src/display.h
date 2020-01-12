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
void show(); // void render(); // 渲染整个画面
void show_information(); // 显示游戏信息（前两行）
void show_foods(); // 显示食物
void show_snakes(); // 显示蛇，获取蛇的节点
void point_to_line(node* head); // 将蛇头、蛇身的节点、蛇尾连接起来
void blank_snake();
void blank_line(node* head);

void setup_aio_buffer(); // 异步IO

void snake_move();

void game_over(int sig);

int set_ticker(int n_msecs);

#endif // DISPLAY_H__
