// Michael Allen Jr.
// mma357

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <time.h>

using std::cout;
using std::endl;
using std::ofstream;

// Pre-defining funcitons used later in the file for use in main.
int handshake(int &clientSocket);
void setupNegotiationSocket(sockaddr_in &server, int &sock_fd, int port);
void setupDataSocket(sockaddr_in &server, int &sock_fd, int port);

int main(int argc, char *argv[]) {

  // seeding the random number generator
  srand(time(NULL));

  struct sockaddr_in server, client;
  int negotiationSocket = 0, clientSocket = 0, dataSocket = 0, negotiationPort, randomPort;
  socklen_t clen = sizeof(client);

  // atoi to convert the port arg to an int
  negotiationPort = atoi(argv[1]);

  // The helper functions defined later inn the file remove a lot of the
  // boilerplate code from main. Here I am using one to set up the
  // negotiation (TCP) socket.
  setupNegotiationSocket(server, negotiationSocket, negotiationPort);
  listen(negotiationSocket, 10);

  if ((clientSocket=accept(negotiationSocket, (struct sockaddr*)&server, (socklen_t*)&client)) == -1)
    cout << "Failed to receive.\n";
  
  // The handshake function takes in a built socket and listens for a message.
  // Upon message reciept, if the handshake is correct a port will be generated
  // and returned and if not the program will exit.
  randomPort = handshake(clientSocket);

  if (randomPort != -1) {
    cout << "\nHandshake detected. Selected the random port " << randomPort << "\n\n";
  }

  // Closing negotiation sockets
  close(negotiationSocket);
  close(clientSocket);

  // Using another one of the helper functions to set up the data socket
  setupDataSocket(server, dataSocket, randomPort);

  char packet[6];
  ofstream dataReceived("dataReceived.txt", std::ios::out);

  if (!dataReceived.is_open()) {
    cout << "Error opening output file\n";
    return -1;
  }

  while(true) {
    recvfrom(dataSocket, (char *)packet, 6, MSG_CONFIRM, (struct sockaddr*)&client, &clen);

    // The incoming packet has two bits of control data at the beginning.
    // The first bit indicates EOF and the second contains the length of
    // the data.
    int i = packet[1] - '0';
    if (i == 0) i = 4;

    for (int x = 2; x < i + 2; x++) {
      dataReceived << packet[x];
      packet[x] = toupper(packet[x]);
    }

    if ((sendto(dataSocket, packet, 6, MSG_CONFIRM, (struct sockaddr*)&client, clen)) == -1) {
      cout << "Failed sending data to data socket\n";
      break;
    }
    
    if (packet[0] == '1') {
      break;
    }
  }

  dataReceived.close();
  close(dataSocket);
  return 0;
}

// Handshake check
int handshake(int &clientSocket) {
  char payload[16];
  char response[16];
  int randomPort;

  read(clientSocket, payload, 16);

  // Comparing sent value to 2020
  if(strcmp(payload, "2020") != 0) {
    cout << payload;
    cout << "Incorrect handshake value.\n";
    send(clientSocket, "-1", 16, 0);
    return -1;
  }

  // generating a port between 65535 and 1024
  randomPort = rand() % (65535 - 1024 + 1);
  snprintf(response, 20, "%d", randomPort);
  send(clientSocket, response, 16, 0);
  return randomPort;
}

void setupNegotiationSocket(sockaddr_in &server, int &sock_fd, int port) {

  if ((sock_fd=socket(AF_INET, SOCK_STREAM, 0)) == -1)
    cout << "Error in socket creation.\n";

  // Initializing TCP Data socket
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
    cout << "Error in binding negotiation socket.\n";
    exit(-1);
  }
}

void setupDataSocket(sockaddr_in &server, int &sock_fd, int port) {

  if ((sock_fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Failed creating data socket.\n";

  // Initializing UDP Data socket
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(port);

  if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
    cout << "Error in binding negotiation socket.\n";
    exit(-1);
  }
}

/*
  Refrences:
  https://www.geeksforgeeks.org/socket-programming-cc/
  https://www.geeksforgeeks.org/udp-server-client-implementation-c/
*/