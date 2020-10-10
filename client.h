// Author: Michael Allen Jr, Wesley Carter
//         mma357            wc609
// Date: Oct 6, 2016

#ifndef CLIENT_H
#define CLIENT_H
 
#include <stdlib.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>
#include <arpa/inet.h>
#include "packet.h"
#include <math.h>
#include <time.h>
#include <sstream>
#include <unistd.h>

using std::cout;
using std::endl;
using std::stringstream;
using std::noskipws;
using std::ifstream;
using std::ios_base;

class Client{
	
private:

  struct sockaddr_in server;
  struct hostent *hostName;
  const char *fileName;
  int negotiationPort, gbnSocket;

public:

	Client(const char *_hostName, const char *_negotiationPort, const char *_fileName) {
    hostName = gethostbyname(_hostName);
    negotiationPort = atoi(_negotiationPort);
    fileName = _fileName;
  }

  int initConnection() {
    memset((char *) &server, 0, sizeof(server));
    bcopy((char *)hostName->h_addr,
    (char *)&server.sin_addr.s_addr,
    hostName->h_length);

    server.sin_port = htons(negotiationPort);
    gbnSocket = socket(AF_INET, SOCK_DGRAM, 0);

    return gbnSocket;
  }

  void sendPacket() {
    packet *_packet;
    char * data = new char[64]();
    memset(data, 0, sizeof(char) * 64);
    _packet = new packet(1, 0, 64, (char *)"im a retard");
    _packet->serialize(data);
    socklen_t slen = sizeof(server);
    if ((sendto(gbnSocket, data, sizeof(data), 0, (struct sockaddr*) &server, slen)) == -1) {
      std::cout << "Failed sedning data to sata socket\n";
      delete _packet;
      delete data;
      exit(1);
    }

    delete _packet;
    delete data;
  }


};
 
#endif