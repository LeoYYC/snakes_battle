#include "serverlib.h"

#include "msg.h"

int make_server_socket(int portnum)
{
	return make_server_socket_q(portnum, BACKLOG);
}

int make_server_socket_q(int portnum, int backlog)
{
	struct sockaddr_in saddr;   /* build our address here */
	//struct hostent* hp;   /* this is part of our    */
	//char hostname[HOSTLEN];     /* address 	         */
	int	sock_id;	       /* the socket             */

	sock_id = socket(PF_INET, SOCK_STREAM, 0);  /* get a socket */
	if (sock_id == -1)
	{
		one_msg(MSG_ERROR, "can not socket");
		return -1;
	}

	/** build address and bind it to socket **/

	//bzero((void*)&saddr, sizeof(saddr));   /* clear out struct     */
	//gethostname(hostname, HOSTLEN);         /* where am I ?         */
	//hp = gethostbyname(hostname);           /* get info about host  */
	//										/* fill in host part    */
	//bcopy((void*)hp->h_addr_list[0], (void*)&saddr.sin_addr, hp->h_length);
	saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port = htons(portnum);        /* fill in socket port  */
	saddr.sin_family = AF_INET;            /* fill in addr family  */
	if (bind(sock_id, (struct sockaddr*) & saddr, sizeof(struct sockaddr_in)) != 0)
	{
		one_msg(MSG_ERROR, "can not bind");
		return -1;
	}

	/** arrange for incoming calls **/

	if (listen(sock_id, backlog) != 0)
	{
		one_msg(MSG_ERROR, "can not listen");
		return -1;
	}
	return sock_id;
}
