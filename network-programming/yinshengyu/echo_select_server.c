#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100

void error_handling(char * message);

int main(int argc, char * argv[])
{
	int server_sock, client_sock;

	struct sockaddr_in server_address, client_address;

	struct timeval timeout;
	
	fd_set reads, cpy_reads;

	socklen_t address_size;

	int fd_max, str_len, fd_num, i;

	char buf[BUF_SIZE];

	if (argc != 2)
	{
	    printf("Usage : %s <port> \n", argv[0]);
	    exit(1);
	}

	server_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&server_address, 0, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
	    error_handling("bind() error");
        
	if (listen(server_sock, 5) == -1)
	    error_handling("listen() error");
	
	FD_ZERO(&reads);
	FD_SET(server_sock, &reads);

	fd_max = server_sock;

        while (1)
	{
	    cpy_reads = reads;
	    timeout.tv_sec = 5;
	    timeout.tv_usec = 5000;

	    if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
	        break;
            
	    if (fd_num == 0)
	        continue;
	    
	    for (i = 0; i < fd_max + 1; i++)
	    {
	        if (FD_ISSET(i, &cpy_reads))
		{
		    address_size = sizeof(client_address);
		    client_sock = accept(server_sock, (struct sockaddr*)&client_address, &address_size);
		    FD_SET(client_sock, &reads);
		    
		    if (fd_max < client_sock)
		        fd_max = client_sock;
		    
		    printf("connected client : %d \n", client_sock);
		}
		else
		{
		    str_len = read(i, buf, BUF_SIZE);
		    if (str_len == 0)
		    {
		        FD_CLR(i, &reads);
			close(i);
		        printf("closed client : %d \n", i);
		    }
		    else
		    {
		        write(i, buf, str_len);
		    }
		}
	    }
	}

	
	close(server_sock);
	return 0;

}


void error_handling( char * message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
