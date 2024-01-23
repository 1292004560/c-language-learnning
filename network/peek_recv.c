#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SZIE 30

void error_handling(char * message);

int main(int argc, char * argv[])
{
	int accept_sock, recv_sock;

	struct sockaddr_in accept_address, recv_address;

	int str_len, state;
	socklen_t recv_address_size;

	char buf[BUF_SZIE];
	if (argc != 2)
	{
		printf("Usage ； %s <port> \n", argv[0]);
		exit(1);
	}

	accept_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&accept_address, 0, sizeof(accept_address));
	accept_address.sin_family = AF_INET;
	accept_address.sin_addr.s_addr = htonl(INADDR_ANY);

	accept_address.sin_port = htons(atoi(argv[1]));

	if (bind(accept_sock, (struct sockaddr *)&accept_address, sizeof(accept_address)) == -1)
	{
		error_handling("bind() error");
	}

	listen(accept_sock, 5);

	recv_address_size = sizeof(recv_address);

	recv_sock = accep(accept_sock, (struct sockaddr *)&recv_address, &recv_address_size);

	while (1)
	{
		str_len = recv(recv_sock, buf, sizeof(buf) - 1, MSG_PEEK|MSG_DONTWAIT);
		if (str_len > 0)
			break;
	}

	buf[str_len] = 0;
	printf("Buffering %d bytes : %s \n", str_len, buf);

	str_len = recv(recv_sock, buf, sizeof(buf) - 1, 0);
	buf[str_len] = 0;
	printf("Read again : %s \n", buf);
	close(accept_sock);
	close(recv_sock);
	return 0;
}

void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
