// Authors: Michael Allen Jr | Wesley Carter
//          mma357           |  wc609
// Date: Oct 6, 2016

#include "server.h"

int main(int argc, char *argv[]) {
  Server *server = new Server(argv[1]);

  if (server->initConnection() == -1) {
    cout << "Error in binding negotiation socket.\n";
    exit(-1);
  }

  ofstream out_file(argv[2], std::ios::out | std::ios::trunc);

  if (!out_file.is_open()) {
    cout << "Failed to open data file\n";
    return -1;
  }

  while(server->receiving()) {
    if (server->recvPacket() == -1) {
      cout << "Error in receiving packet.\n";
      exit(-1);
    }

    if (server->packetCorrupt()) {
      continue;
    }

    out_file.write(
      server->extractPacketData(),
      server->extractPacketLength()
    );

    if (server->receivedPacketIsEot()) {
      if (server->endTransmission() < 0) {
        cout << "Error sending ack packet.\n";
        exit(-1);
      }
    } else {
      if (server->acknowledge() < 0) {
        cout << "Error sending ack packet.\n";
        exit(-1);
      }
    }

    server->log(); 
  }

  cout << "-----------------------------------------" << endl;

  out_file.close();
  return 0;
}


/*
    //////////////////////////////////////////////////////

                      PUBLIC METHODS
                      
    //////////////////////////////////////////////////////
*/


// Server constructor: command args are directly passed in and
// parsed locally. This was done for better readability of the
// main function. The server length is also calculated here and
// stored as a member variable for use later in the program.
Server::Server(const char *_negotiationPort) {
  negotiationPort = atoi(_negotiationPort);
  slen = sizeof(server);
}


// Server destructor: since the packets used in the client are
// dynamically allocated, the associated memory needs to be freed
// upon program exit. That is what's happening here.
Server::~Server() {
  if (ackPacket != nullptr) {
    delete ackPacket;
  }

  if (dataPacket != nullptr) {
    delete dataPacket;
  }
}

// Initializing UDP connection: the udp server is a member of the class
// so this initiializer function instantiates the requested connection
// and makes the server available to members of the class.
int Server::initConnection() {
  if ((gbnSocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Failed creating data socket.\n";

  // Initializing UDP Data socket
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(negotiationPort);

  return bind(gbnSocket, (struct sockaddr *)&server, sizeof(server));
}


// Recieve packet: waits to receive an ack packet from the client.
int Server::recvPacket() {
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


// Extract packet data: returns the character array stored in the
// last received packet if it exists and is a data packet.
char * Server::extractPacketData() {
  if (dataPacket == nullptr && dataPacket->getType() == 1) {
    return (char *) "";
  }

  return dataPacket->getData();
}


// Extract packet length: returns the length varibale stored in the
// last received packet if it exists and is a data packet.
int Server::extractPacketLength() {
  if (dataPacket == nullptr && dataPacket->getType() == 1) {
    return 0;
  }

  return dataPacket->getLength();
}


// End transmission: pretty self-explanatory. Sends EOT packet to client.
int Server::endTransmission() {
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


// Acknowledge: sends an ack packet back to the client
// with the expected sequence number.
int Server::acknowledge() {
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


// Receiveing: a control function that will keep the while loop running
// until an EOT packet is received from the client.
bool Server::receiving() {
  return !done;
}


// Received packet is EOT: if the last received packet is an EOT, this one is true.
bool Server::receivedPacketIsEot() {
  return dataPacket->getType() == 3;
}


// Packet corrput: if the sequence number is out of bounds the packet is corrupt.
bool Server::packetCorrupt() {
    return dataPacket->getSeqNum() >= 8;
  }


// Log: logging function required by assignment. It's only here so we won't
// get fined.
void Server::log() {
  cout << "\n--------------------------------------" << endl;
  dataPacket->printContents();
  cout << "Expecting Rn:  " << ackPacket->getSeqNum() << endl;
  cout << "sn: " << ackPacket->getSeqNum() << endl;
  ackPacket->printContents();
}

