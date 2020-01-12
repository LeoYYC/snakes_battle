#include "msg.h"

void one_msg(char* msg_type, char* msg)
{
	printf("[%s]: %s\n", msg_type, msg);
}

void two_msg(char* msg_type, char* msg1, char* msg2)
{
	printf("[%s]: %s %s\n", msg_type, msg1, msg2);
}

void error_msg(char* msg)
{
	printf("[%s]: ", MSG_ERROR);
	perror(msg);
}