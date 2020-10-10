#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include "packet.h"

using namespace std;

void error(string message)
{
	cout << message << endl;
}

int main()
{
	struct sockaddr_in server;
	struct sockaddr_in client;
	int mysocket = 0;
	int portNum = 0;
	socklen_t clen = sizeof(client);
	socklen_t slen = sizeof(server);
	string fileName;

	cout << "serverPort: UDP port number used by the server to receive data from the client: ";
	cin >> portNum;
	cout << endl;
	cout << "fileName: name of the file into which the received data is written: ";
	cin >> fileName;

	if (mysocket = socket(AF_INET, SOCK_DGRAM, 0) == -1)
	{
		error("Error in socket creation");
		return 1;
	}

	memset((char*)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(portNum);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(mysocket, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		error("Error in binding");
		return 1;
	}

  
	return 0;
}