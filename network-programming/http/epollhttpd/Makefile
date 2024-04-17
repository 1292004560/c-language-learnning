all: httpd 

httpd: epoll.c main.c  epoll.h
	gcc -W -Wall -g  epoll.c main.c  epoll.h -o httpd

clean:
	rm httpd