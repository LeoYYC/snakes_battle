#include "display.h"

#include "client.h"
#include <pthread.h>

int input_char;
struct coordinate dir;
int move_count = 0;
int done = 0;

pthread_mutex_t snakedata_mutex = PTHREAD_MUTEX_INITIALIZER;

int start()
{
	initscr();
	init_game();

	//signal(SIGIO, get_input);
	//enable_kbd_signals();
	signal(SIGALRM, show);
	set_ticker(RENDER_DELAY);

	get_input();

	//nocbreak(); // 把终端的CBREAK模式关闭，缓冲
	//echo(); // 回显
	//curs_set(1); // 把光标置为可见
	//keypad(stdscr, false); // 关闭功能键响应模式

	//while (!done)
	//{
	//	pause();
	//}

	endwin();
	return 0;
}

int init_game()
{
	cbreak(); // 把终端的CBREAK模式打开，禁用缓冲
	noecho(); // 关闭回显
	curs_set(0); // 把光标置为不可见
	keypad(stdscr, true); // 开启功能键响应模式

	dir.xloc = 1;
	dir.yloc = 0;
	init_me_snake(COLS, LINES);
	
	set_ticker(RENDER_DELAY);
}

void get_input()
//void get_input(int signum)
{
	while (1)
	{
		input_char = getch();
		if (KEY_LEFT == input_char)
		{
			dir.xloc = 0;
			dir.yloc = -1;
		}
		else if (KEY_UP == input_char)
		{
			dir.xloc = -1;
			dir.yloc = 0;
		}
		else if (KEY_RIGHT == input_char)
		{
			dir.xloc = 0;
			dir.yloc = 1;
		}
		else if (KEY_DOWN == input_char)
		{
			dir.xloc = 1;
			dir.yloc = 0;
		}
		else if ('Q' == input_char)
		{
			done = 1;
		}
		set_ticker(RENDER_DELAY);
	}
}

void show()
{
	signal(SIGALRM, show);
	clear();

	show_information();
	show_foods();
	
	/*move_count++;
	if (move_count % 4 == 0)
	{
		move_count = 0;
		snake_move();
	}*/
	snake_move();
	show_snakes();
	
	refresh();
}

void snake_move()
{
	node* head = get_me_snake();
	node* tail = get_me_snake_tail();
	//如果蛇头砬到自己的身体，则游戏结束
	//if (SNAKE_HEAD == mvinch(head->next->yloc + dir.yloc, head->next->xloc + dir.xloc))
	//{
	//	//game_over(2);
	//	return;
	//}
	//blank_snake();
	node* temp = head->next;
	if (temp->next->next == NULL)
	{ // temp->next = tail
		temp->xloc += dir.xloc;
		temp->yloc += dir.yloc;
	}
	else
	{
		int xminus = temp->xloc - temp->next->xloc;
		int yminus = temp->yloc - temp->next->yloc;
		if (xminus == 0)
		{
			if (dir.xloc == 0)
			{
				//int fd = fopen("error.txt", "w");
				//char* errortxt = "if (xminus == 0){if (dir.xloc == 0)";
				////write(fd, errortxt, 37);
				//fprintf(fd, "%s, %d", errortxt, dir.yloc);
				//fclose(fd);
				if (yminus / dir.yloc < 0)
				{
					//game_over(2);
					return;
				}
				else
				{
					temp->yloc += dir.yloc;
				}
			}
			else
			{
				insert_node(temp->xloc + dir.xloc, temp->yloc, head);
			}
		}
		else if (yminus == 0)
		{
			if (dir.yloc == 0)
			{
				//int fd = fopen("error.txt", "w");
				//char* errortxt = "if (yminus == 0){if (dir.yloc == 0)";
				////write(fd, errortxt, 36);
				//fprintf(fd, "%s, %d", errortxt, dir.xloc);
				//fclose(fd);
				if (xminus / dir.xloc < 0)
				{
					//game_over(2);
					return;
				}
				else
				{
					temp->xloc += dir.xloc;
				}
			}
			else
			{
				insert_node(temp->xloc, temp->yloc + dir.yloc, head);
			}
		}
		else
		{
			printw("coordinate is wrong");
			return;
		}
		xminus = tail->pre->xloc - tail->pre->pre->xloc;
		yminus = tail->pre->yloc - tail->pre->pre->yloc;
		if (xminus == 0)
		{
			if (yminus < 0)
			{
				tail->pre->yloc++;
			}
			else
			{
				tail->pre->yloc--;
			}
			if (tail->pre->xloc == tail->pre->pre->xloc  && tail->pre->yloc == tail->pre->pre->yloc)
			{
				delete_node(tail);
			}
		}
		else if (yminus == 0)
		{
			if (xminus < 0)
			{
				tail->pre->xloc++;
			}
			else
			{
				tail->pre->xloc--;
			}
			if (tail->pre->xloc == tail->pre->pre->xloc && tail->pre->yloc == tail->pre->pre->yloc)
			{
				delete_node(tail);
			}
		}
	}
	
	upload_me_snake_data(get_server_socket_fd());
}

//void blank_snake()
//{
//	node* me = get_me_snake();
//	node** other_snakes = get_other_snakes();
//	blank_line(me);
//	int i;
//	for (i = 0; i < MAX_PLAYERS_PER_ROOM - 1; i++)
//	{
//		blank_line(other_snakes[i]);
//	}
//}
//
//void blank_line(node* head)
//{
//	if (head == NULL)
//	{
//		return;
//	}
//	int i;
//	node* temp = head->next;
//	while (temp->next != NULL)
//	{
//		if (temp->next->next == NULL)
//		{
//			mvaddch(temp->xloc, temp->yloc, ' ');
//		}
//		else
//		{
//			int shape_num;
//			int min_loc;
//			if (temp->xloc == temp->next->xloc)
//			{ // temp->next = tail
//				if (temp->next->yloc < temp->yloc)
//				{
//					min_loc = temp->next->yloc;
//					shape_num = temp->yloc - temp->next->yloc;
//				}
//				else
//				{
//					min_loc = temp->yloc;
//					shape_num = temp->next->yloc - temp->yloc;
//				}
//				for (i = 0; i < shape_num; i++)
//				{
//					mvaddch(temp->xloc, min_loc + i, ' ');
//				}
//			}
//			else if (temp->yloc == temp->next->yloc)
//			{
//				if (temp->next->xloc < temp->xloc)
//				{
//					min_loc = temp->next->xloc;
//					shape_num = temp->xloc - temp->next->xloc;
//				}
//				else
//				{
//					min_loc = temp->xloc;
//					shape_num = temp->next->xloc - temp->xloc;
//				}
//				for (i = 0; i < shape_num; i++)
//				{
//					mvaddch(min_loc + i, temp->yloc, ' ');
//				}
//			}
//			else
//			{
//				printw("coordinate is wrong");
//				return;
//			}
//		}
//
//		temp = temp->next;
//	}
//}

void show_information()
{
	move(1, 0);
	int i;
	for (i = 0; i < COLS; i++)
		addstr("-");
	move(0, COLS / 2 - 5);
	printw("length: %d", get_my_snake_length());
}

void show_foods()
{
	int i;
	struct coordinate* tmp_foods_loc = get_foods_loc();
	for (i = 0; i < FOODS_NUM; i++)
	{
		mvaddch(tmp_foods_loc[i].xloc, tmp_foods_loc[i].yloc, FOOD_SHAPE);
	}
}

void show_snakes()
{
	node* me = get_me_snake();
	node** other_snakes = get_other_snakes();
	point_to_line(me);
	int i;
	for (i = 0; i < MAX_PLAYERS_PER_ROOM; i++)
	{
		pthread_mutex_lock(&snakedata_mutex);
		point_to_line(other_snakes[i]);
		pthread_mutex_unlock(&snakedata_mutex);
	}
}

void point_to_line(node* head)
{
	if (head == NULL)
	{
		return;
	}
	int i;
	node* temp = head->next;
	while (temp->next != NULL)
	{
		if (temp->next->next == NULL)
		{
			mvaddch(temp->xloc, temp->yloc, temp->shape);
		}
		else
		{
			int shape_num;
			int min_loc;
			if (temp->xloc == temp->next->xloc)
			{ // temp->next = tail
				if (temp->next->yloc < temp->yloc)
				{
					min_loc = temp->next->yloc;
					shape_num = temp->yloc - temp->next->yloc;
				}
				else
				{
					min_loc = temp->yloc;
					shape_num = temp->next->yloc - temp->yloc;
				}
				for (i = 0; i < shape_num; i++)
				{
					mvaddch(temp->xloc, min_loc + i, temp->shape);
				}
				mvaddch(temp->xloc, temp->next->yloc, temp->shape);
			}
			else if (temp->yloc == temp->next->yloc)
			{
				if (temp->next->xloc < temp->xloc)
				{
					min_loc = temp->next->xloc;
					shape_num = temp->xloc - temp->next->xloc;
				}
				else
				{
					min_loc = temp->xloc;
					shape_num = temp->next->xloc - temp->xloc;
				}
				for (i = 0; i < shape_num; i++)
				{
					mvaddch(min_loc + i, temp->yloc, temp->shape);
				}
				mvaddch(temp->next->xloc, temp->yloc, temp->shape);
			}
			else
			{
				printw("coordinate is wrong");
				return;
			}
		}
		
		temp = temp->next;
	}
}

void game_over(int sig)
{
	// 显示结束原因
	move(0, 0);
	int j;
	for (j = 0; j < COLS; j++)
		addstr(" ");
	move(0, 2);
	if (1 == sig)
		addstr("Crash the wall. Game over");
	else if (2 == sig)
		addstr("Crash itself. Game over");
	else if (3 == sig)
		addstr("Mission Complete");
	set_ticker(0); // 关闭计时器
	delete_link(get_me_snake(), get_me_snake_tail()); // 释放链表的空间
}

//void enable_kbd_signals()
//{
//	int fd_flags;
//	fcntl(0, F_SETOWN, getpid());
//	fd_flags = fcntl(0, F_GETFL);
//	fcntl(0, F_SETFL, (fd_flags | O_ASYNC));
//}

int set_ticker(int n_msecs)
{
	struct itimerval new_timeset;
	long n_sec, n_usecs;

	n_sec = n_msecs / 1000;
	n_usecs = (n_msecs % 1000) * 1000L;

	new_timeset.it_interval.tv_sec = n_sec;        /* set reload  */
	new_timeset.it_interval.tv_usec = n_usecs;      /* new ticker value */
	new_timeset.it_value.tv_sec = n_sec;      /* store this   */
	new_timeset.it_value.tv_usec = n_usecs;     /* and this     */

	return setitimer(ITIMER_REAL, &new_timeset, NULL);
}