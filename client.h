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
#include "common.h"

using std::cout;
using std::endl;
using std::stringstream;
using std::noskipws;
using std::ifstream;
using std::ios_base;
// using std::nullptr_t;

class Client{
	
private:

  SequenceCounter ns, sb;
  struct sockaddr_in server;
  struct hostent *hostName;
  int negotiationPort, gbnSocket;
  socklen_t slen;
  time_t start_t, end_t;
  double diff_t;

  packet *window[8];
  char   data[8][30 + 1];
  char chunk[64];
  packet *ackPacket, *eotPacket;

  bool timerRunning;
  bool done;

  void deletePacket(int index) {
    if (window[index] != nullptr) {
      delete window[index];
    }
  }

  packet* setPacket(int index, int type, int seqNum, int length, char * value) {
    this->deletePacket(index);

    window[index] = new packet(type, seqNum, length, value);
    return window[index];
  }

  void startTimer() {
    time(&start_t);
  }

  void stopTimer() {
    timerRunning = false;
  }

  void log(packet *_packet) {
    cout << "\n--------------------------------------" << endl;
    _packet->printContents();
    cout << "SB: " << sb.value << endl;
    cout << "NS: " << ns.value << endl;
    cout << "Number of outstanding packets: " << ns.distance(sb);
  }

  void logEot() {
    ackPacket->printContents();
    cout << "EOT packet received. Exiting.--------------------------------------" << endl;
  }

public:

	Client(const char *_hostName, const char *_negotiationPort) {
    hostName = gethostbyname(_hostName);
    negotiationPort = atoi(_negotiationPort);
    slen = sizeof(server);
  }

  ~Client() {
    for (int i = 0; i < 8; ++i) {
      this->deletePacket(i);
    }

    if (eotPacket != nullptr) {
      delete eotPacket;
    }

    if (ackPacket != nullptr) {
      delete ackPacket;
    }
  }

  int initConnection() {
    memset((char *) &server, 0, sizeof(server));
    bcopy((char *)hostName->h_addr,
    (char *)&server.sin_addr.s_addr,
    hostName->h_length);

    server.sin_port = htons(negotiationPort);
    gbnSocket = socket(AF_INET, SOCK_DGRAM, 0);

    struct timeval ackTimeout;
    ackTimeout.tv_sec = 0;
    ackTimeout.tv_usec = 40000;
    setsockopt(gbnSocket, SOL_SOCKET, SO_RCVTIMEO, &ackTimeout, sizeof ackTimeout);

    return gbnSocket;
  }

  int outstandingPackets() {
    return ns.distance(sb);
  }

  bool windowFull() {
    return outstandingPackets() >= 7;
  }

  int recvPacket() {
    memset((char *) &chunk, 0, sizeof(chunk));

    if (ackPacket != nullptr) {
      delete ackPacket;
    }

    ackPacket = new packet(0, 0, 0, (char *) "");
    if ((recvfrom(gbnSocket, chunk, 64, 0, (struct sockaddr*) &server, &slen)) < 0) {
      return -1;
    }

    ackPacket->deserialize(chunk);
    sb = ackPacket->getSeqNum() + 1;

    if (ackPacket->getType() == 2) {
      stopTimer();
      done = true;
      logEot();
      return 0;
    }

    log(ackPacket);

    if (sb.value == ns.value) {
      stopTimer();
    } else {
      startTimer();
    }
  }

  bool timerExpired() {
    time(&end_t);
    diff_t = difftime(end_t, start_t);

    return diff_t > 10.0 && timerRunning;
  }

  void resendPackets() {
    for(int i = sb.value; i < ns.distance(sb); ++i) {
      sendPacket(i);
    }
  }

  int createPacket(char * value, int len = 30) {
    memset(data[ns.value], 0,  sizeof(data[ns.value]));
    strncpy(data[ns.value], value, sizeof(char) * len);
    packet * _packet = this->setPacket(ns.value, 1, ns.value, len, data[ns.value]);
    ns++;

    return _packet->getSeqNum();
  }

  void sendPacket(int index) {

    if (window[index] == nullptr) {
      return;
    }

    memset((char *) &chunk, 0, sizeof(chunk));

    packet * _packet = window[index];
    _packet->serialize(chunk);

    if ((sendto(gbnSocket, chunk, sizeof(chunk), 0, (struct sockaddr*) &server, slen)) == -1) {
      std::cout << "Failed sending data to data socket\n";
      exit(1);
    }

    if (index == sb.value) {
      cout << "timer restart" << endl;
      timerRunning = true;
      startTimer();
    }

    log(_packet);
  }

  bool sending() {
    return !done;
  }

  int endTransmission() {
    memset((char *) &chunk, 0, sizeof(chunk));

    if (eotPacket != nullptr) {
      delete eotPacket;
    }

    eotPacket = new packet(3, ns.value, 0, NULL);
    eotPacket->serialize(chunk);

    cout << "\n--------------------------------------" << endl;
    cout << "Sending EOT packet to server." << endl;
    eotPacket->printContents();

    if ((sendto(gbnSocket, chunk, sizeof(chunk), 0, (struct sockaddr*) &server, slen)) == -1) {
      std::cout << "Failed sending eot packet to data socket\n";
      return -1;
    }
  }

  bool windowFilled() {
    return ns.value == 7 || ns.cycle > 0;
  }

};
 
#endif