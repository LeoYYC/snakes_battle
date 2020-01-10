#ifndef clientlib_h__
#define clientlib_h__

#include	<stdio.h>
#include	<unistd.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<time.h>
#include	<strings.h>

int connect_to_server(char* host, int portnum);

#endif // clientlib_h__


