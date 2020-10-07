all: client server

client: client.o
		g++ -std=c++11 client.cpp client.h packet.cpp packet.h -o client

server: server.o
		g++ -std=c++11 server.cpp server.h packet.cpp packet.h -o server

clean:
		\rm *.o client server
