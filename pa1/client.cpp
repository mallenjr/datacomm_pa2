// Michael Allen Jr.
// mma357

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using std::cout;
using std::endl;
using std::stringstream;
using std::noskipws;
using std::ifstream;
using std::ios_base;

// Pre-defining funcitons used later in the file for use in main.
void handshake(struct sockaddr_in &server, int &clientSocket, int &randomPort);
void setupDataSocket(sockaddr_in &server, int &sock_fd, int port);
void setupNegotiationSocket(sockaddr_in &server, int &sock_fd, int port, hostent * hostname);

int main(int argc, char *argv[]) {

  struct sockaddr_in server;
  int negotiationSocket = 0, dataSocket =  0, negotiationPort, randomPort;
  socklen_t slen = sizeof(server);


  // Storing the hostname for later use
  struct hostent *hostName;
  hostName = gethostbyname(argv[1]);


  // atoi to convert the port arg to an int
  negotiationPort = atoi(argv[2]);


  // The helper functions defined later inn the file remove a lot of the
  // boilerplate code from main. Here I am using one to set up the
  // negotiation (TCP) socket.
  setupNegotiationSocket(server, negotiationSocket, negotiationPort, hostName);


  // The handshake function takes in a built socket and listens for a message.
  // Upon message reciept, if the handshake is correct a port will be returned
  // from the server.
  handshake(server, negotiationSocket, randomPort);


  // Closing negotiation sockets
  close(negotiationSocket);

  if (randomPort == -1) {
    cout << "Handshake failed\n";
    return -1;
  }


  // Using another one of the helper functions to set up the data socket
  setupDataSocket(server, dataSocket, randomPort);

  // This sleep period ensures that the first packet gets delivered.
  // (at least while testing)
  usleep(200);

  // opening the requested file. if file does not exist remote server is closed
  ifstream data;
  data.open(argv[3], ios_base::in | ios_base::binary);
  if (!data.is_open()) {
    sendto(dataSocket, "10", 6, MSG_CONFIRM, (struct sockaddr*) &server, slen);
    char chunk[6];
    recvfrom(dataSocket, chunk, 6, MSG_CONFIRM, (struct sockaddr*) &server, &slen);
    close(dataSocket);
    cout << "Failed to open data file\n";
    return -1;
  }

  char chunk[4];
  char packet[6];
  bool fileParsed = false;
  int i = 0;

  while(!fileParsed) {
    i = 0;
    memset((char *) &packet, 0, sizeof(packet));
    memset((char *) &chunk, 0, sizeof(chunk));
    packet[1] = packet[0] = '0';


    // Read the file 4 bytes at a time while keeping track of the number
    // of characters read
    while(data >> noskipws >> chunk[i]) {
      packet[i + 2] = chunk[i];
      i++;
      if (i == 4)
        break;
    }

    // if EOF is reached or will be reached next packet the client indicates
    // this in the packet
    if (data.eof() || data.peek() == EOF) {
      packet[0] = '1';
      packet[1] = '0' + i;
      fileParsed = true;
    }


    // packet is sent
    if ((sendto(dataSocket, packet, 6, MSG_CONFIRM, (struct sockaddr*) &server, slen)) == -1) {
      std::cout << "Failed sedning data to sata socket\n";
      break;
    }

    // Ack is recieved
    if ((recvfrom(dataSocket, packet, 6, MSG_CONFIRM, (struct sockaddr*) &server, &slen)) > 0) {
      cout << packet[2] << packet[3] << packet[4] << packet[5] << endl;
    }
  }

  data.close();
  close(dataSocket);
  return 0;
}


// Handshake check
void handshake(struct sockaddr_in &server, int &clientSocket, int &randomPort) {
  char payload[16] = "2020";
  char response[16];

  if (connect(clientSocket, (struct sockaddr*) &server, sizeof(server)) == -1) {
    cout << "Error establishing connection to server for handshake.\n";
    randomPort = -1;
  }
  
  // Sending 2020
  send(clientSocket, payload, 16, 0);
  read(clientSocket, response, 16);

  if (strcmp(response, "-1") == 0) {
    cout << "Invalid handshake value.\n";
    randomPort = -1;
  }

  // parsing response
  sscanf(response, "%d", &randomPort);
}


// Initializing TCP Data socket
void setupNegotiationSocket(sockaddr_in &server, int &sock_fd, int port, hostent * hostName) {

  if ((sock_fd=socket(AF_INET, SOCK_STREAM, 0)) == -1)
    cout << "Error in socket creation.\n";

  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  bcopy((char *)hostName->h_addr,
  (char *)&server.sin_addr.s_addr,
  hostName->h_length);
}


// Initializing UDP Data socket
void setupDataSocket(sockaddr_in &server, int &sock_fd, int port) {

  server.sin_port = htons(port);

  if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    cout << "Error creating data socket\n";
    exit(-1);
  }

  struct timeval read_timeout;
  read_timeout.tv_sec = 0;
  read_timeout.tv_usec = 500;
  // setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
}

/*
  Refrences:
  https://www.geeksforgeeks.org/socket-programming-cc/
  https://www.geeksforgeeks.org/udp-server-client-implementation-c/
*/