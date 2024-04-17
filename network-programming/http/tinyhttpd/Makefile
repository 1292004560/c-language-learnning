all: httpd client

httpd: httpd.c
	gcc -W -Wall -g -pthread -o httpd httpd.c


client: simple-client.c
	gcc -W -Wall -o simple-client simple-client.c



clean:
	rm httpd
	rm simple-client
