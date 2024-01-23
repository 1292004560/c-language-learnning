#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char * message);
void read_child_process(int sig);

int main(int argc, char * argv[])
{
    int server_sock, client_sock;

    struct sockaddr_in server_address, client_address;

    pid_t pid;

    struct sigaction action;

    socklen_t address_size;

    int str_len, state;

    char buf[BUF_SIZE];

    if (argc != 2)
    {
        printf("Usage : %s <port> \n", argv[0]);
	exit(1);
    }

    action.sa_handler = read_child_process;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    
    state = sigaction(SIGCHLD, &action, 0);

    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    server_address.sin_port = htons(atoi(argv[1]));

    if (bind(server_sock, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
         error_handling("bind() error");
    if (listen(server_sock, 5) == -1)
         error_handling("listen() error");
 
 
    while (1)
    {
         address_size = sizeof(client_address);
         
	 client_sock = accept(server_sock, (struct sockaddr*)&client_address, &address_size);
	 
	 if (client_sock == -1)
	     continue;
	 else
	     puts("new client connected.....");

	 pid = fork();

	 if (pid == -1)
	 {
	     close(client_sock);
	     continue;
	 }

	 if (pid == 0)
	 {
	      close(server_sock);
	      while ((str_len = read(client_sock, buf, BUF_SIZE)) != 0)
	             write(client_sock, buf, str_len);
	      
	      close(client_sock);

	      puts("client disconnected....");

	      return 0;
	 }
	 else
	      close(client_sock);

    }
    close(server_sock);

    return 0;
}



void read_child_process(int sig)
{
    pid_t pid;

    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc  id : %d \n", pid);
}

void error_handling(char * message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}



