#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SZIE 100
#define EPOLL_SIZE 50

void error_handling(char * message);

int main(int argc, char * argv[])
{
	int server_sock, client_sock;

	struct sockaddr_in server_addr, client_addr;

	socklen_t addr_size;

	int str_len, i;
	char buf[BUF_SZIE];

	struct epoll_event * events;
	struct epoll_event event;

	int epfd, event_cnt;

	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	server_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		error_handling("bind() error");
	
	if (listen(server_sock, 5) == -1)
		error_handling("listen() error");
	
	epfd = epoll_create(EPOLL_SIZE);
	events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	event.events = EPOLLIN;
	event.data.fd = server_sock;

	epoll_ctl(epfd, EPOLL_CTL_ADD, server_sock, &event);

	while (1)
	{
		event_cnt = epoll_wait(epfd, events, EPOLL_SIZE, -1);

		if (event_cnt == -1)
		{
			puts("epoll_wait() error");
			break;
		}

		for (i = 0; i < event_cnt; i++)
		{
			if (events[i].data.fd == server_sock)
			{
				addr_size = sizeof(client_addr);
				client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
				event.events = EPOLLIN;
				event.data.fd = client_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, client_sock, &event);
				printf("conneted client : %d\n", client_sock);
			}
			else
			{
				str_len = read(events[i].data.fd, buf, BUF_SZIE);
				if (str_len == 0)
				{
					epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
					close(events[i].data.fd);
					printf("close client : %d \n", events[i].data.fd);
				}
				else
				{
					write(events[i].data.fd, buf, str_len);
				}
			} 
		}
	}
	close(server_sock);
	close(epfd);
	return 0;
}

void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}
