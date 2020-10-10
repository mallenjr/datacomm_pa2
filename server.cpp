// Author: Michael Allen Jr, Wesley Carter
//         mma357            wc609
// Date: Oct 6, 2016

#include "server.h"

int main(int argc, char *argv[]) {
  Server *server = new Server(argv[1], argv[2]);

  if (server->initConnection() == -1) {
    cout << "Error in binding negotiation socket.\n";
    exit(-1);
  }

  return 0;
}

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

int Server::listen() {
  
}
