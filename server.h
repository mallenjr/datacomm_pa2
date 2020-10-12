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
#include "common.h"

using std::cout;
using std::endl;
using std::stringstream;
using std::noskipws;
using std::ofstream;
using std::ios_base;

class Server{
	
private:

  SequenceCounter ns;
  struct sockaddr_in server;
  struct hostent *hostName;
  const char *fileName;
  int negotiationPort, gbnSocket;
  packet * ackPacket, * dataPacket;
  socklen_t slen;
  bool done = false;

  char data[31];
  char chunk[64];

public:

	Server(const char *_negotiationPort) {
    negotiationPort = atoi(_negotiationPort);
    slen = sizeof(server);
  }

  ~Server() {
    if (ackPacket != nullptr) {
      delete ackPacket;
    }

    if (dataPacket != nullptr) {
      delete dataPacket;
    }
  }

  int initConnection() {
    if ((gbnSocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
      cout << "Failed creating data socket.\n";

    // Initializing UDP Data socket
    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(negotiationPort);

    return bind(gbnSocket, (struct sockaddr *)&server, sizeof(server));
  }

  int recvPacket() {
    char chunk[64];

    memset((char *) &data, 0, sizeof(data));
    memset((char *) &chunk, 0, sizeof(chunk));

    if (dataPacket != nullptr) {
      delete dataPacket;
    }

    dataPacket = new packet(1, 0, 30, data);
    if ((recvfrom(gbnSocket, chunk, 64, 0, (struct sockaddr*) &server, &slen)) < 0) {
      return -1;
    }

    dataPacket->deserialize(chunk);
  }

  bool packetCorrupt() {
    return dataPacket->getSeqNum() >= 8;
  }

  char * extractPacketData() {
    if (dataPacket == nullptr) {
      return (char *) "";
    }

    return dataPacket->getData();
  }

  int extractPacketLength() {
    if (dataPacket == nullptr) {
      return 0;
    }

    return dataPacket->getLength();
  }

  int endTransmission() {
    if (ackPacket != nullptr) {
      delete ackPacket;
    }

    ackPacket = new packet(2, ns.value, 0, NULL);
    ackPacket->serialize(chunk);

    if ((sendto(gbnSocket, chunk, 64, 0, (struct sockaddr*) &server, slen)) < 0) {
      return -1;
    }

    done = true;
  }

  int acknowledge() {
    memset((char *) &chunk, 0, sizeof(chunk));

    if (ackPacket != nullptr) {
      delete ackPacket;
    }

    ackPacket = new packet(0, ns.value, 0, NULL);
    ackPacket->serialize(chunk);

    if ((sendto(gbnSocket, chunk, 64, 0, (struct sockaddr*) &server, slen)) < 0) {
      return -1;
    }

    ns++;
    return 0;
  }

  bool receiving() {
    return !done;
  }

  bool receivedPacketIsEot() {
    return dataPacket->getType() == 3;
  }

  void log() {
    cout << "\n--------------------------------------" << endl;
    dataPacket->printContents();
    cout << "Expecting Rn:  " << ackPacket->getSeqNum() << endl;
    cout << "sn: " << ackPacket->getSeqNum() << endl;
    ackPacket->printContents();
  }

};