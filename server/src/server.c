#include "server.h"

#include "msg.h"

int prefock_pids[PREFORK];
int sockpair[PREFORK][2];

int prefork();
int sub_process();
int rsv_socket();
int snd_socket();
int send_fd(int fd, int* sendfd);
int recv_fd(int fd, int* recvfd);
int x_sock_set_block(int sock, int on);

int accept_client()
{
	int sock, fd;
	FILE* fpin;
	char request[BUFSIZ]; // default buffer size
	sock = make_server_socket(PORTNUM);
	if (sock == -1)
	{
		one_msg(MSG_ERROR, "can not make_server_socket");
		exit(2);
	}

	int i;
	for (i = 0; i < 10; i++) {
		if (fork() == 0)
			break;
	}

	/* main loop here */

	while (1) {
		/* take a call and buffer it */
		struct sockaddr_in client_addr;
		int client_addr_len;
		fd = accept(sock, (struct sockaddr*) & client_addr, (socklen_t*)&client_addr_len);
		
		//fd = accept(sock, NULL, NULL);

		int clientip = client_addr.sin_addr.s_addr;
		printf("[%s]: client socket address: %d.%d.%d.%d:%d\n", MSG_INFO, clientip & 255, (clientip >> 8) & 255,
			(clientip >> 16) & 255, (clientip >> 24) & 255, ntohs(client_addr.sin_port));

		fpin = fdopen(fd, "r");
		/* read request */
		fgets(request, BUFSIZ, fpin);
		printf("[%s]: got a call: request = %s\n", MSG_INFO, request);
		fclose(fpin);

		//int n;
		//while ((n = read(fd, request, BUFSIZ)) > 0)
		//{
		//	printf("got a call: request = %s\n", request);
		//}
		//close(fd);
	}

	return 0;
}

//int prefork()
//{
//	int i = 0;
//	int rv_fork;
//	int rlt;
//	for (i = 0; i < PREFORK; i++) {
//
//		rlt = socketpair(AF_UNIX, SOCK_STREAM, 0, sockpair[i]);
//		if (rlt < 0) {
//			printf("Call socketpair error, errno is %d\n", errno);
//			return errno;
//		}
//
//		if ((rv_fork = fork()) == -1) {
//			continue;
//		}
//
//		if (0 == rv_fork) {
//			/* in child */
//			close(sockpair[i][1]);
//
//			for (; ; ) {
//				rlt = x_sock_set_block(sockpair[i][0], 1);
//				if (rlt != 0) {
//					printf("[CHILD]: x_sock_set_block error, errno is %d\n", rlt);
//					break;
//				}
//
//				int fd;
//				rlt = recv_fd(sockpair[i][0], &fd);
//				if (rlt < 0) {
//					printf("[CHILD]: recv_fd error, errno is %d\n", rlt);
//					break;
//				}
//
//
//				if (fd < 0) {
//					printf("[CHILD]: child process exit normally!\n");
//					break;
//				}
//
//
//				/* 处理fd描述符对应的文件 */
//				//rlt = write(fd, MY_LOGO, strlen(MY_LOGO));
//				if (rlt < 0) {
//					printf("[CHILD]: write error, errno is %d\n", rlt);
//				}
//				else {
//					printf("[CHILD]: append logo successfully\n");
//				}
//				close(fd);
//			}
//		}
//		else
//		{
//			prefock_pids[i] = rv_fork;
//			
//			wait(NULL);
//		}
//	}
//}
//
//int sub_process()
//{
//	return 0;
//}
//
//int rsv_socket()
//{
//	return 0;
//}
//
//int snd_socket()
//{
//	return 0;
//}
//
//int send_fd(int fd, int* sendfd)
//{
//	struct iovec iov[1];
//	struct msghdr msg;
//	char buf[100];
//
//	msg.msg_name = NULL;
//	msg.msg_namelen = 0;
//
//	union {
//		struct cmsghdr cm;
//		char control[CMSG_SPACE(sizeof(int))];
//	}control_un;
//
//	// 设置辅助缓冲区和长度
//	msg.msg_control = control_un.control;
//	msg.msg_controllen = sizeof(control_un.control);
//
//	// 只需要一组附属数据就够了，直接通过 CMSG_FIRSTHDR 取得
//	struct cmsghdr* cmptr = CMSG_FIRSTHDR(&msg);
//	// 设置必要的字段，数据和长度
//	cmptr->cmsg_len = CMSG_LEN(sizeof(int)); // fd 类型是 int ，设置长度
//	cmptr->cmsg_level = SOL_SOCKET;
//	cmptr->cmsg_type = SCM_RIGHTS;  // 指明发送的是描述符
//
//	iov[0].iov_base = buf;
//	iov[0].iov_len = 100;
//	msg.msg_iov = iov;
//	msg.msg_iovlen = 1;
//
//	*((int*)CMSG_DATA(cmptr)) = *sendfd; // 把dup出来的fd写入辅助数据中
//
//	if (sendmsg(fd, &msg, 0) < 0) {
//		printf("sendmsg error, errno is %d\n", errno);
//		return errno;
//	}
//
//	return 0;
//}
//
//int recv_fd(int fd, int* recvfd)
//{
//	struct iovec iov[1];
//	struct msghdr msg;
//	char buf[100];
//
//	msg.msg_name = NULL;
//	msg.msg_namelen = 0;
//
//	union {
//		struct cmsghdr cm;
//		char control[CMSG_SPACE(sizeof(int))];
//	}control_un;
//
//	struct cmsghdr* cmptr;
//
//	// 设置辅助缓冲区和长度
//	msg.msg_control = control_un.control;
//	msg.msg_controllen = sizeof(control_un.control);
//
//	iov[0].iov_base = buf;
//	iov[0].iov_len = 100;
//	msg.msg_iov = iov;
//	msg.msg_iovlen = 1;
//
//
//	if (recvmsg(fd, &msg, 0) < 0) {
//		return errno;
//	}
//
//	if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL && (cmptr->cmsg_len == CMSG_LEN(sizeof(int)))) {
//		if (cmptr->cmsg_level != SOL_SOCKET) {
//			printf("cmsg_leval is not SOL_SOCKET\n");
//			return -1;
//		}
//
//		if (cmptr->cmsg_type != SCM_RIGHTS) {
//			printf("cmsg_type is not SCM_RIGHTS");
//			return -1;
//		}
//
//		*recvfd = *((int*)CMSG_DATA(cmptr));
//		printf("recv fd = %d\n", recvfd);
//
//		//char* testmsg = "test msg";
//		//write(*recvfd, testmsg, strlen(testmsg) + 1);
//	}
//	else
//	{
//		*recvfd = -1;
//	}
//
//
//	return 0;
//}
//
//int x_sock_set_block(int sock, int on)
//{
//	int val;
//	int rv;
//
//	val = fcntl(sock, F_GETFL, 0);
//	if (on) {
//		rv = fcntl(sock, F_SETFL, ~O_NONBLOCK & val);
//	}
//	else {
//		rv = fcntl(sock, F_SETFL, O_NONBLOCK | val);
//	}
//
//	if (rv) {
//		return errno;
//	}
//
//	return 0;
//}
