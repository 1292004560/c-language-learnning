all: server client
server:
	g++ util.h util.cpp server.cpp  -o server

client:
	g++ util.h util.cpp client.cpp -o client

clean:
	rm -rf server && rm -rf client
