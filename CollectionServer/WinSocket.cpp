#include "WinSocket.h"


WinSocket::WinSocket()
{
   // Perform general Socket Overhead
   m_pSocketInfo = new addrinfo;

   // create WSADATA object
   WSADATA wsaData;

   // Initialize the socket in memory.
   m_Socket = INVALID_SOCKET;

   // Initialize Winsock. WSA.. Windows Socket API
   // Loads the winsock dll.
   m_iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (m_iResult != 0)
   {
      //printf("WSAStartup failed with error: %d\n", m_iResult);
      exit(1);
   }

   // Initialize the m_SocketInfo 'hints'
   ZeroMemory(m_pSocketInfo, sizeof(*m_pSocketInfo));
   m_pSocketInfo->ai_socktype = SOCK_STREAM;
   m_pSocketInfo->ai_protocol = IPPROTO_TCP;    // TCP connection!!!
   //hints.ai_flags = AI_PASSIVE; // server

}


WinSocket::~WinSocket()
{
}


int WinSocket::isReady()
{
   return m_iResult;
}
