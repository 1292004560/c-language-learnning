#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

void error_handling(char * message);

int main(int argc, char * message)
{
	int sock;
	char message[BUF_SIZE];

	int str_len;
	struct sockaddr_in server_addr;

	FILE * readfp;
	FILE * writefp;

	if (argc != 3)
	{
		printf("Usage : %s<IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM,0);

	if (sock == -1)
		error_handling("socket() error");

	memset(&server_addr,0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		error_handling("connect() error!");
	
	else
		puts("Conneted .............");
	
	readfp = fdopen(sock, "r");
	writefp = fdopen(sock, "w");

	while (1)
	{
		fputs("Input message(Q to quit) :", stdout);
		fgets(message, BUF_SIZE, stdin);
		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		fputs(message, writefp);
		fflush(writefp);
		fgets(message, BUF_SIZE, readfp);
		printfs("Message from server : %s\n", message);
	}

	fclose(writefp);
	fclose(readfp);
	return 0;

}

void error_handling(char * message)
{
}
