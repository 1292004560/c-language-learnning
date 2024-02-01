#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024

int main(int argc, char * argv[])
{
	int server_sock, client_sock;
	FILE * readfp;
	FILE * writefp;

	struct sockaddr_in server_addr, client_addr;
	
	printf(argv[0]);

	socklen_t client_addr_size;
	char buf[BUF_SIZE] = {0,};

	server_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	bind(server_sock, (struct sockaddr * )&server_addr, sizeof(server_addr));

	listen(server_sock, 5);
	client_addr_size = sizeof(client_addr);
	client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);

	readfp = fdopen(client_sock, "r");
	writefp = fdopen(client_sock, "w");
	fputs("FORM SERVER : Hi ~client?\n", writefp);

	fputs("I love all of the world \n", writefp);

	fputs("You are awesome !\n", writefp);

	fflush(writefp);
	fclose(writefp);

	fgets(buf, sizeof(buf), readfp);
	fputs(buf, stdout);
	fclose(readfp);
	return 0;
}
