#pragma once
#include <WinSock2.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "6881"
#define DEFAULT_IP_ADDRESS "127.0.0.1"
#define PACKET_SIZE 1024

class WinSocket
{
public:
   WinSocket();
   virtual ~WinSocket();

protected:
   // Returns 0 if initialization was successful.
   int isReady();

   // Store whether initialization was successful
   int m_iResult;

   // Wrap the socket.
   SOCKET m_Socket;

   // Stores socket info, not the concrete address.
   // Stores address family, etc.
   addrinfo* m_pSocketInfo;
};

