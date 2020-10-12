// Author: Michael Allen Jr, Wesley Carter
//         mma357            wc609
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
