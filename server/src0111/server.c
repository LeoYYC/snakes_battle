#include "server.h"

#include "utils/msg.h"
#include "utils/cJSON.h"

int rooms_pids[PREFORK];
int sockpair[PREFORK][2];
char* fname = "socketfd_convey";

int room_process();
int sub_process();
int rsv_socket();
int snd_socket();
int send_fd(int fd, int* sendfd);
int recv_fd(int fd, int* recvfd);
int x_sock_set_block(int sock, int on);

int accept_client()
{
	int sock, client_fd;
	FILE* fpin;
	char request[BUFSIZ]; // default buffer size
	sock = make_server_socket(PORTNUM);
	if (sock == -1)
	{
		one_msg(MSG_ERROR, "can not make_server_socket");
		exit(2);
	}

	int i;
	int socketpair_rlt;
	int fork_rlt;
	for (i = 0; i < MAX_ROOMS; i++) {
		socketpair_rlt = socketpair(AF_UNIX, SOCK_STREAM, 0, sockpair[i]);
		if (socketpair_rlt < 0) {
			printf("[%s]: Call socketpair error, errno is %d\n", MSG_ERROR, errno);
			return errno;
		}
		if ((fork_rlt = fork()) == 0)
		{
			/* in child */
			close(sockpair[i][1]);

			for (; ; ) {
				socketpair_rlt = x_sock_set_block(sockpair[i][0], 1);
				if (socketpair_rlt != 0) {
					printf("[%s]: [CHILD]: x_sock_set_block error, errno is %d\n", MSG_ERROR, socketpair_rlt);
					break;
				}

				int fd;
				socketpair_rlt = recv_fd(sockpair[i][0], &fd);
				int client_fd = dup(fd);
				close(fd);

				if (socketpair_rlt < 0) {
					printf("[%s]: [CHILD]: recv_fd error, errno is %d\n", MSG_ERROR, socketpair_rlt);
					break;
				}

				if (client_fd < 0) {
					printf("[%s]: [CHILD]: child process exit normally!\n", MSG_SUCCESS);
					break;
				}

				char buf[BUFSIZ];
				int n;
				while ((n = read(client_fd, buf, BUFSIZ)) > 0)
				{
					printf("%d %s\n", n, buf);
					bzero(buf, BUFSIZ);
				}				
			}
			break;
		}
		else if (fork_rlt > 0)
		{
			rooms_pids[i] = fork_rlt;
			//wait(0);
		}
		else
		{
			continue;
		}
	}

	/* main loop here */
	if (fork_rlt > 0)
	{
		for (i = 0; i < PREFORK; i++) {
			if (fork() == 0)
				break;
		}
		while (1) {
			/* take a call and buffer it */
			struct sockaddr_in client_addr;
			int client_addr_len;
			client_fd = accept(sock, (struct sockaddr*) & client_addr, (socklen_t*)&client_addr_len);

			//fd = accept(sock, NULL, NULL);

			int clientip = client_addr.sin_addr.s_addr;
			printf("[%s]: client socket address: %d.%d.%d.%d:%d\n", MSG_INFO, clientip & 255, (clientip >> 8) & 255,
				(clientip >> 16) & 255, (clientip >> 24) & 255, ntohs(client_addr.sin_port));

			//fpin = fdopen(client_fd, "r");
			///* read request */
			//fgets(request, BUFSIZ, fpin);
			//printf("[%s]: got a call: request = %s\n", MSG_INFO, request);
			//fclose(fpin);

			int n;
			cJSON* json = NULL;
			cJSON* string_key = NULL;

			read(client_fd, request, BUFSIZ);
			printf("[%s]: got a call: request = %s\n", MSG_INFO, request);

			//将字符串解析为一个结构体
			json = cJSON_Parse(request);
			string_key = cJSON_GetObjectItem(json, "join_room");

			//printf("[%s]: join_room is %d\n", MSG_INFO, string_key->valueint);

			int dup_fd = dup(client_fd);
			if (string_key->valueint < MAX_ROOMS && string_key->valueint > 0)
			{
				send_fd(sockpair[string_key->valueint][1], &dup_fd);
			}

			cJSON_Delete(json);

			close(client_fd);
		}
	}
	else if (fork_rlt == 0)
	{
		//room_process();
	}
	else
	{
		one_msg(MSG_INFO, "There is a lose fork.");
	}

	return 0;
}

int send_fd(int fd, int* sendfd)
{
	struct iovec iov[1];
	struct msghdr msg;
	char buf[100];

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	}control_un;

	// 设置辅助缓冲区和长度
	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	// 只需要一组附属数据就够了，直接通过 CMSG_FIRSTHDR 取得
	struct cmsghdr* cmptr = CMSG_FIRSTHDR(&msg);
	// 设置必要的字段，数据和长度
	cmptr->cmsg_len = CMSG_LEN(sizeof(int)); // fd 类型是 int ，设置长度
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;  // 指明发送的是描述符

	iov[0].iov_base = buf;
	iov[0].iov_len = 100;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	*((int*)CMSG_DATA(cmptr)) = *sendfd; // 把dup出来的fd写入辅助数据中

	if (sendmsg(fd, &msg, 0) < 0) {
		printf("sendmsg error, errno is %d\n", errno);
		return errno;
	}

	return 0;
}

int recv_fd(int fd, int* recvfd)
{
	struct iovec iov[1];
	struct msghdr msg;
	char buf[100];

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	}control_un;

	struct cmsghdr* cmptr;

	// 设置辅助缓冲区和长度
	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	iov[0].iov_base = buf;
	iov[0].iov_len = 100;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;


	if (recvmsg(fd, &msg, 0) < 0) {
		return errno;
	}

	if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL && (cmptr->cmsg_len == CMSG_LEN(sizeof(int)))) {
		if (cmptr->cmsg_level != SOL_SOCKET) {
			printf("cmsg_leval is not SOL_SOCKET\n");
			return -1;
		}

		if (cmptr->cmsg_type != SCM_RIGHTS) {
			printf("cmsg_type is not SCM_RIGHTS");
			return -1;
		}

		*recvfd = *((int*)CMSG_DATA(cmptr));
		//printf("[%s]: recv fd = %d\n", MSG_INFO, *recvfd);

		//char* testmsg = "test msg";
		//write(*recvfd, testmsg, strlen(testmsg) + 1);
	}
	else
	{
		*recvfd = -1;
	}


	return 0;
}

int x_sock_set_block(int sock, int on)
{
	int val;
	int rv;

	val = fcntl(sock, F_GETFL, 0);
	if (on) {
		rv = fcntl(sock, F_SETFL, ~O_NONBLOCK & val);
	}
	else {
		rv = fcntl(sock, F_SETFL, O_NONBLOCK | val);
	}

	if (rv) {
		return errno;
	}

	return 0;
}
