// Author: Michael Allen Jr, Wesley Carter
//         mma357            wc609
// Date: Oct 6, 2016

#include "client.h"

int main(int argc, char *argv[]) {
  packet *_packet;
  Client *client = new Client(argv[1], argv[2], argv[3]);

  client->initConnection();

  while (true) {
    // _packet = new packet(1, 0, 100, "im a retard");
    client->sendPacket();
    sleep(1);
    // delete _packet;
  }
  return 0;
}

