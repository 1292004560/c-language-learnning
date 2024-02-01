#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SZIE 1024


int main(int argc, char * argv[])
{
	int sock;
	char buf[BUF_SZIE];

	struct sockaddr_in server_addr;

	FILE * readfp;
	FILE * writefp;
	sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	
	readfp = fdopen(sock, "r");
	writefp = fdopen(sock, "w");

	while (1)
	{
		if (fgets(buf, sizeof(buf), readfp) == NULL)
			break;
		fputs(buf, stdout);
		fflush(stdout);

	}

	fputs("FORM CLIENT : Thank you !\n", writefp);
	fflush(writefp);
	fclose(writefp);
	fclose(readfp);
	return 0;
}
