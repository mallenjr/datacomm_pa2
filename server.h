// Author: Michael Allen Jr, Wesley Carter
//         mma357            wc609
// Date: Oct 6, 2016

#include <stdlib.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <fstream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "packet.h"
#include <unistd.h>

using std::cout;
using std::endl;
using std::stringstream;
using std::noskipws;
using std::ifstream;
using std::ios_base;

class Server{
	
private:

  struct sockaddr_in server;
  struct hostent *hostName;
  const char *fileName;
  int negotiationPort, gbnSocket;
  packet *_packet;

public:

	Server(const char *_negotiationPort, const char *_fileName) {
    negotiationPort = atoi(_negotiationPort);
    fileName = _fileName;
    _packet = new packet(0, 0, 64, (char *)"");
  }

  ~Server() {
    delete _packet;
  }

  int initConnection();
  int listen();

};