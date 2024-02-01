#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SZIE 4
#define EPOLL_SIZE 50

void error_handling(char * message);

int main(int argc, char * argv[])
{
	int server_sock, client_sock;
	struct sockaddr_in server_addr, client_addr;

	socklen_t addr_size;

	char buf[BUF_SZIE];

	struct epoll_event *ep_events;
	struct epoll_event event;
	int epollfd, event_count;

	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	server_sock = socket(PF_INET， SOCK_STREAM, 0);
	memset(&server_sock, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		error_handling("bind() error");
	
	if (listen(server_sock, 5) == -1)
		error_handling("listen() error");
	
	epollfd = epoll_create(EPOLL_SIZE);
	ep_events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	event.events = EPOLLIN;
	event.data.fd = server_sock;

	epoll_ctl(epollfd, EPOLL_CTL_ADD, server_sock, -1);

	while (1)
	{
		event_count = epoll_wait(epollfd, ep_events, EPOLL_SIZE, -1);

		if (event_count == -1)
		{
			puts("epoll_wait() error");
			break;
		}
		
		puts("return epoll_wait()");

		for (i = 0; i < event_count; i++)
		{
			addr_size = sizeof(client_addr);
			client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
			event.events = EPOLLIN;
			event.data.fd = client_sock;
			epoll_ctl(epollfd, EPOLL_CTL_ADD, client_sock, &event);
			printf("conneted client : %d \n", client_sock);
		}

		else
		{
			str_len = read(ep_events[i].data.fd, buf, BUF_SZIE);
			if (str_len == 0)
			{
				epoll_ctl(epollfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
				close(ep_events[i].data.fd);
				printf("closed client : %d \n", ep_events[i].data.fd);
			}
			else
			{
				write(ep_events[i].data.fd, buf, str_len);
			}
		}
	}
	close(server_sock);
	close(epollfd);
	return 0;
}

void error_handling(char * message)
{
}
