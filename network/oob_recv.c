#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUF_SIZE 30

void error_handling(char * message);
void urgent_handler(int signo);

int accept_sock;
int recv_sock;

int main(int argc, char * argv[])
{
	struct sockaddr_in recv_address, server_address;

	int str_len, state;

	socklen_t server_address_size;
	struct sigaction action;

	char buf[BUF_SIZE];
	if (argc != 2)
	{
		printf("usage : %s <port>\n", argv[0]);
		exit(1);
	}

	action.sa_handler = urgent_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	accept_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&recv_address, 0, sizeof(recv_address));
	recv_address.sin_family = AF_INET;
	recv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	recv_address.sin_port = htons(atoi(argv[1]));

	if (bind(accept_sock, (struct sockaddr *)&recv_address, sizeof(recv_address)) == -1)
	{
		error_handling("bind() error");
	}
	listen(accept_sock, 5);

	server_address_size = sizeof(server_address);
	recv_sock = accept(accept_sock, (struct sockaddr *)&server_address, &server_address_size);

	fcntl(recv_sock, F_SETOWN, getpid());
	state = sigaction(SIGURG, &action, 0);

	while ((str_len = recv(recv_sock, buf, sizeof(buf), 0)) != 0)
	{
		if (str_len == -1)
			continue;
		buf[str_len] = 0;
		puts(buf);
	}

	close(recv_sock);
	close(accept_sock);
	
	return 0;

}

void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void urgent_handler(int signo)
{
	int str_len;
	char buf[BUF_SIZE];
	str_len = recv(recv_sock, buf, sizeof(buf) -1, MSG_OOB);
	buf[str_len] = 0;
	printf("Urgent message : %s \n", buf);
}
