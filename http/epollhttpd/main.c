#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "epoll.h"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("eg. port resource_path\n");
        exit(1);
    }
    int port = atoi(argv[1]);
    int ret = chdir(argv[2]);
    if (ret == -1)
    {
        perror("chdir error");
        exit(1);
    }

    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(lfd, (struct sockaddr *)&address, sizeof(address));

    if (ret == -1)
    {
        perror("bind err");
        exit(1);
    }
    listen(lfd, 64);
    epoll_run(lfd);
}