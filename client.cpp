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
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include "packet.h"
#include "client.h"
#include <math.h>
#include <time.h>

using std::cout;
using std::endl;
using std::stringstream;
using std::noskipws;
using std::ifstream;
using std::ios_base;

int main(int argc, char *argv[]) {
  return 0;
}

Client::Client(const char *_hostName, const char *_negotiationPort, const char *_fileName) {
  hostName = gethostbyname(_hostName);
  negotiationPort = atoi(_negotiationPort);
  fileName = _fileName;
}