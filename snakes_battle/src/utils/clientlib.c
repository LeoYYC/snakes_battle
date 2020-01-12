#include "clientlib.h"

#include "msg.h"

int connect_to_server(char* host, int portnum)
{
	int sock;
	struct sockaddr_in servadd;        /* the number to call */
	struct hostent* hp;            /* used to get number */

	/** Step 1: Get a socket **/

	sock = socket(AF_INET, SOCK_STREAM, 0);    /* get a line   */
	if (sock == -1)
	{
		one_msg(MSG_ERROR, "can not socket");
		return -1;
	}

	/** Step 2: connect to server **/

	bzero(&servadd, sizeof(servadd));     /* zero the address     */
	hp = gethostbyname(host);             /* lookup host's ip #   */
	if (hp == NULL)
		return -1;
	//bcopy(hp->h_addr, (struct sockaddr*)&servadd.sin_addr, hp->h_length);
	bcopy(hp->h_addr_list[0], (struct sockaddr*)&servadd.sin_addr, hp->h_length);
	servadd.sin_port = htons(portnum);      /* fill in port number  */
	servadd.sin_family = AF_INET;          /* fill in socket type  */

	if (connect(sock, (struct sockaddr*)&servadd, sizeof(servadd)) != 0)
	{
		one_msg(MSG_ERROR, "can not connect");
		return -1;
	}

	return sock;
}