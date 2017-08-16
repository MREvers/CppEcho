#pragma once
#include <WinSock2.h>
#include <map>
#include <string>
#include <vector>
#include "WinSocket.h"

class ServerSocket: public WinSocket
{
public:
    ServerSocket(PCSTR iport = DEFAULT_PORT);
   ~ServerSocket();

   // Begins listening on the Port
   int Begin();

   // Returns true if succesful connection. Stores connection id in id.
   bool AcceptNewConnection(unsigned int id);

   std::vector<unsigned int> GetClients();
   int ReceiveData(unsigned int aclient_id, char* arecvbuf, unsigned int aiBuffsize);
   void SendTo(unsigned int aiClient, const std::string& aszMsg);

private:
   void closeCollection(unsigned int aiClient);

   // table to keep track of each client's socket
   // Since this accepts SOCKET and not SOCKET*, the SOCKET will be copied in.
   std::map<unsigned int, SOCKET> m_Sessions;
};

