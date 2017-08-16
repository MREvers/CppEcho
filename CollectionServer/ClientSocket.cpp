#include "ClientSocket.h"
#include <iostream>
#include <WS2tcpip.h>

ClientSocket::ClientSocket(PCSTR aszIP, PCSTR aszPORT) : WinSocket()
{
   if (isReady() == 0)
   {
      m_IP   = aszIP;
      m_Port = aszPORT;
   }
}


ClientSocket::~ClientSocket()
{
}


bool ClientSocket::BeginConnect()
{
   struct addrinfo *result, *ptr;

   //resolve server address and port 
   // This port IP is used to designate the target because it will be used in 'connect'
   m_pSocketInfo->ai_family = AF_UNSPEC;
   m_iResult = getaddrinfo(m_IP, m_Port, m_pSocketInfo, &result);

   if (m_iResult != 0)
   {
      printf("getaddrinfo failed with error: %d\n", m_iResult);
      WSACleanup();
      exit(1);
   }

   // Attempt to connect to an address until one succeeds
   for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
   {
      SOCKET socketTest = INVALID_SOCKET;
      // Create a SOCKET for connecting to server
      socketTest = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

      if (socketTest == INVALID_SOCKET)
      {
         printf("socket failed with error: %ld\n", WSAGetLastError());
         WSACleanup();
         exit(1);
      }

      // Connect to server.
      m_iResult = connect(socketTest, ptr->ai_addr, (int)ptr->ai_addrlen);

      if (m_iResult == SOCKET_ERROR)
      {
         int error = WSAGetLastError();
         closesocket(socketTest);
         socketTest = INVALID_SOCKET;
         printf("The server is down... did not connect. " + error);
      }
      else
      {
         m_Socket = socketTest;
      }
   }

   // no longer need address info for server
   freeaddrinfo(result);

   // if connection failed
   if (m_Socket == INVALID_SOCKET)
   {
      printf("Unable to connect to server!\n");
      WSACleanup();
      exit(1);
   }

   // Set the mode of the socket to be nonblocking
   u_long iMode = 1;

   m_iResult = ioctlsocket(m_Socket, FIONBIO, &iMode);
   if (m_iResult == SOCKET_ERROR)
   {
      printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
      closesocket(m_Socket);
      WSACleanup();
      exit(1);
   }

   //disable nagle
   char value = 1;
   setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

   return false;
}

int ClientSocket::ReceiveData(char * recvbuf, unsigned int aiBufsize)
{
   m_iResult = recv(m_Socket, recvbuf, aiBufsize, 0);

   if (m_iResult == 0)
   {
      std::printf("Connection closed\n");
      closesocket(m_Socket);
      WSACleanup();
      exit(1);
   }

   return m_iResult;
}

int ClientSocket::SendData(std::string aszMsg)
{
   m_iResult = send(m_Socket, aszMsg.c_str(), aszMsg.size(), 0);

   if (m_iResult == 0)
   {
      std::printf("Connection closed\n");
      closesocket(m_Socket);
      WSACleanup();
      exit(1);
   }

   return m_iResult;
}