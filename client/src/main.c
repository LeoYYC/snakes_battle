#include <stdio.h>
#include "display.h"
#include "client.h"

int main(int ac, char* av[])
{
	if (ac == 1)
	{
		connect_to(SERVER_IP, SERVER_PORT);
	}
	else if (ac == 3)
	{
		connect_to(av[1], av[2]);
	}
	else
	{
		perror("usage: char* host, int portnum");
		exit(1);
	}
	join_room(1);
	while (0)
	{
		printf("Which room do you want to join? [1-9] [0 to quit]\n");
		int roomid;
		scanf("%d", &roomid);
		if (roomid <= 0)
		{
			printf("ByeBye~\n");
			return 0;
		}
		while (join_room(roomid))
		{
			printf("This room is full, please choose another one. [1-9] [0 to quit]\n");
			scanf("%d", &roomid);
			if (roomid <= 0)
			{
				printf("ByeBye~\n");
				return 0;
			}
		}
		printf("What is your nickname? [less than 20 characters] [0 to quit]\n");
		char nname[20]; // nick name
		scanf("%s", nname);
		if (nname[0] == '0' && nname[1] == '\0')
		{
			printf("ByeBye~\n");
			return 0;
		}
		while (add_player(nname))
		{
			printf("This name is used, please choose another one. [less than 20 characters] [0 to quit]\n");
			scanf("%s", nname);
			if (nname[0] == '0' && nname[1] == '\0')
			{
				printf("ByeBye~\n");
				return 0;
			}
		}

		int play_rlt = start();
		char again_sign;
		while (!play_rlt)
		{
			printf("Do you wanna play again? [y/n]\n");
			scanf("%c", again_sign);
			if (again_sign == 'y')
			{
				play_rlt = start();
			}
			else
			{
				break;
			}
		}
	}
	start();
	return 0;
}