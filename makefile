CXXFLAGS = -std=c++11
all: client server

client: client.cpp client.h packet.cpp packet.h common.h

server: server.cpp server.h packet.cpp packet.h common.h

clean:
		\rm *.o client server
