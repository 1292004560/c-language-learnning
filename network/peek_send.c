#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

void error_handling(char * message);

int main(int argc, char * argv[])
{
	int sock;
	struct sockaddr_in send_address;

	if (argc != 3)
	{
		printf("Usage : %s <IP> <port> \n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&send_address, 0 , sizeof(send_address));
	send_address.sin_family = AF_INET;
	send_address.s_addr = inet_addr(argv[1]);
	send_address.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr *)&send_address, sizeof(send_address)) == -1)
	{
		error_handling("connect() error!");
	}

	write(sock, "123", strlen("123"));
	close(sock);
	return 0;
}

void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
