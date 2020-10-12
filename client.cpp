// Author: Michael Allen Jr, Wesley Carter
//         mma357            wc609
// Date: Oct 6, 2016

#include "client.h"

int main(int argc, char *argv[]) {
  Client *client = new Client(argv[1], argv[2]);

  int count = 0;

  client->initConnection();
  ifstream in_file;
  char data[31];
  
  in_file.open(argv[3], ios_base::in | ios_base::binary);
  if (!in_file.is_open()) {
    cout << "Failed to open data file\n";
    return -1;
  }

  while (client->sending()) {
    if (!client->windowFull() && !in_file.eof()) {
      memset((char *) &data, 0, sizeof(data));

      in_file.read(data, 30);

      int seqNumber = client->createPacket(data, in_file.gcount());
      client->sendPacket(seqNumber);
    }

    if (client->timerExpired()) {
      client->resendPackets();
      continue;
    }

    if (client->windowFilled()) {
      client->recvPacket();
    }

    if (client->outstandingPackets() == 0) {
      client->endTransmission();
    }
  }

  delete client;
  return 0;
}

