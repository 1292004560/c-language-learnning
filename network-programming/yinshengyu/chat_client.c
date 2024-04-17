#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>

#define BUF_SZIE 100
#define NAME_SIZE 20

void * send_message(void * arg);
void * receive_message(void * arg);

void error_handling(char * msg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SZIE];

int main(int argc, char * argv[])
{
	int sock;
	struct sockaddr_in server_addr;
	pthread_t send_thread, recv_thread;

	void * thread_return;

	if (argc  != 4)
	{
		printf("Usage : %s <IP> <port> <name> \n", argv[0]);
		exit(1);
	}

	sprintf(name, "[%s]", argv[3]);
	sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		error_handling("connect() error");
	
	pthread_create(&send_thread, NULL, send_message, (void*)&sock);
	pthread_create(&recv_thread, NULL, receive_message, (void*)&sock);

	pthread_join(send_thread, &thread_return);
	pthread_join(recv_thread, &thread_return);

	close(sock);
	return 0;
}

void * send_message(void * arg)
{
	int sock = *((int*)arg);
	char name_msg[NAME_SIZE + BUF_SZIE];
	while (1)
	{
		fgets(msg, BUF_SZIE, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			close(sock);
			exit(0);
		}
		sprintf(name_msg, "%s %s", name, msg);
		write(sock, name_msg, strlen(name_msg));
	}

	return NULL;
}

void * receive_message(void * arg)
{
	int sock = *((int*)arg);

	char name_msg[NAME_SIZE + BUF_SZIE];
	int str_len;

	while (1)
	{
		str_len = read(sock, name_msg, NAME_SIZE + BUF_SZIE -1);
		if (str_len == -1)
			return (void*) - 1;
		name_msg[str_len] = 0;
		fputs(name_msg, stdout);
	}

	return NULL;
}

void error_handling(char * message)
{
	
}
