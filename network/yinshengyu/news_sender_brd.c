#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SZIE 30

void error_handling(char * message);

int main(int argc, char * message)
{
	int send_sock;
	struct sockaddr_in  broad_addr;
	FILE * fp;

	char buf[BUF_SZIE];
	int so_brd = 1;

	if (argc != 3)
	{
		printf("Usage : %s <Boradcast IP> <PORT>\n", argv[0]);
		exit(1);
	}
	send_sock = socket(PF_INET, SOCK_DGRAM, 0);
	memset(&broad_addr, 0, sizeof(broad_addr));
	broad_addr.sin_family = AF_INET;
	broad_addr.sin_addr.s_addr = inet_addr(argv[1]);
	broad_addr.sin_port = htons(atoi(argv[2]));
	
	setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void *)&so_brd, sizeof(so_brd));
	if ((fp = fopen("news.txt", "r")) == NULL)
		error_handling("fopen() error ");
	
	while (!feof(fp))
	{
		fgets(buf, BUF_SZIE, fp);
		sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr *)&broad_addr, sizeof(broad_addr));
		sleep(2);
	}

	close(send_sock);
	fclose(fp);
	return 0;
}

void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
