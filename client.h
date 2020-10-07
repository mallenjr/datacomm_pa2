// Author: Michael Allen Jr, Wesley Carter
//         mma357            wc609
// Date: Oct 6, 2016

#ifndef CLIENT_H
#define CLIENT_H
 
class Client{
	
private:

  struct sockaddr_in server;
  struct hostent *hostName;
  const char *fileName;
  int negotiationPort;

public:

	Client(const char *_hostName, const char *_negotiationPort, const char *_fileName);

};
 
#endif