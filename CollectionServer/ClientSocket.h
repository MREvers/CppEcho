#pragma once
#include <WinSock2.h>
#include <string>
#include "WinSocket.h"

// Used for testing purposes.
class ClientSocket: public WinSocket
{
public:
   ClientSocket(PCSTR aszIP = DEFAULT_IP_ADDRESS, PCSTR aszPORT = DEFAULT_PORT);
   ~ClientSocket();
   
   bool BeginConnect();

   int ReceiveData(char * recvbuf, unsigned int aiBufsize);
   int SendData(std::string aszMsg);

private:
   PCSTR m_IP;
   PCSTR m_Port;
};

