#include "epoll_server.h"
#include "utils/config.h"

#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

#define NI_MAXHOST 1025
#define NI_MAXSERV 32
#define NI_NUMERICHOST 1
#define NI_NUMERICSERV 2

#define MAX_EVENT 20
#define READ_BUF_LEN 256

/**
 * ���� file describe Ϊ������ģʽ
 * @param fd �ļ�����
 * @return ����0�ɹ�������-1ʧ��
 */
static int make_socket_non_blocking(int fd)
{
	int flags, s;
	// ��ȡ��ǰflag
	flags = fcntl(fd, F_GETFL, 0);
	if (-1 == flags) {
		perror("Get fd status");
		return -1;
	}

	flags |= O_NONBLOCK;

	// ����flag
	s = fcntl(fd, F_SETFL, flags);
	if (-1 == s) {
		perror("Set fd status");
		return -1;
	}
	return 0;
}

int epoll_server()
{
	// epoll ʵ�� file describe
	int epfd = 0;
	int listenfd = 0;
	int result = 0;
	struct epoll_event ev, event[MAX_EVENT];
	// �󶨵ĵ�ַ
	const char* const local_addr = "192.168.24.128";
	struct sockaddr_in server_addr = { 0 };

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listenfd) {
		perror("Open listen socket");
		return -1;
	}

	/* Enable address reuse */
	int on = 1;
	// �� socket �˿ڸ���, ��ֹ���Ե�ʱ����� Address already in use
	result = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (-1 == result) {
		perror("Set socket");
		return 0;
	}

	server_addr.sin_family = AF_INET;
	//inet_aton(local_addr, &(server_addr.sin_addr));
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORTNUM);
	result = bind(listenfd, (const struct sockaddr*) & server_addr, sizeof(server_addr));
	if (-1 == result) {
		perror("Bind port");
		return 0;
	}
	result = make_socket_non_blocking(listenfd);
	if (-1 == result) {
		return 0;
	}

	result = listen(listenfd, 200);
	if (-1 == result) {
		perror("Start listen");
		return 0;
	}

	// ����epollʵ��
	epfd = epoll_create1(0);
	if (1 == epfd) {
		perror("Create epoll instance");
		return 0;
	}

	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET /* ��Ե����ѡ� */;
	// ����epoll���¼�
	result = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

	if (-1 == result) {
		perror("Set epoll_ctl");
		return 0;
	}

	for (; ; ) {
		int wait_count;
		// �ȴ��¼�
		wait_count = epoll_wait(epfd, event, MAX_EVENT, -1);

		for (int i = 0; i < wait_count; i++) {
			uint32_t events = event[i].events;
			// IP��ַ����
			char host_buf[NI_MAXHOST];
			// PORT����
			char port_buf[NI_MAXSERV];

			int __result;
			// �ж�epoll�Ƿ�������
			if (events & EPOLLERR || events & EPOLLHUP || (!events & EPOLLIN)) {
				printf("Epoll has error\n");
				close(event[i].data.fd);
				continue;
			}
			else if (listenfd == event[i].data.fd) {
				// listen �� file describe �¼������� accept �¼�

				for (; ; ) { // ���ڲ����˱�Ե����ģʽ��������Ҫʹ��ѭ��
					struct sockaddr in_addr = { 0 };
					socklen_t in_addr_len = sizeof(in_addr);
					int accp_fd = accept(listenfd, &in_addr, &in_addr_len);
					if (-1 == accp_fd) {
						perror("Accept");
						break;
					}
					__result = getnameinfo(&in_addr, sizeof(in_addr),
						host_buf, sizeof(host_buf) / sizeof(host_buf[0]),
						port_buf, sizeof(port_buf) / sizeof(port_buf[0]),
						NI_NUMERICHOST | NI_NUMERICSERV);

					if (!__result) {
						printf("New connection: host = %s, port = %s\n", host_buf, port_buf);
					}

					__result = make_socket_non_blocking(accp_fd);
					if (-1 == __result) {
						return 0;
					}

					ev.data.fd = accp_fd;
					ev.events = EPOLLIN | EPOLLET;
					// Ϊ��accept�� file describe ����epoll�¼�
					__result = epoll_ctl(epfd, EPOLL_CTL_ADD, accp_fd, &ev);

					if (-1 == __result) {
						perror("epoll_ctl");
						return 0;
					}
				}
				continue;
			}
			else {
				// �����¼�Ϊ file describe ���Զ�ȡ
				int done = 0;
				// ��Ϊ���ñ�Ե����������������Ҫʹ��ѭ���������ʹ��ѭ�������򲢲�����ȫ��ȡ����������������ݡ�
				for (; ;) {
					ssize_t result_len = 0;
					char buf[READ_BUF_LEN] = { 0 };

					result_len = read(event[i].data.fd, buf, sizeof(buf) / sizeof(buf[0]));

					if (-1 == result_len) {
						if (EAGAIN != errno) {
							perror("Read data");
							done = 1;
						}
						break;
					}
					else if (!result_len) {
						done = 1;
						break;
					}

					write(STDOUT_FILENO, buf, result_len);
				}
				if (done) {
					printf("Closed connection\n");
					close(event[i].data.fd);
				}
			}
		}

	}
	close(epfd);
	return 0;
}