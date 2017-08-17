#include "ServerSocket.h"
#include <iostream>
#include <ws2tcpip.h>

ServerSocket::ServerSocket(PCSTR iport) : WinSocket()
{
   // Check if initialization was successful
   if (isReady() == 0)
   {
      m_Socket = INVALID_SOCKET;

      // Specialize Socket.. AI_PASSIVE means that the Socket will be used in a bind call.
      // See MS website
      m_pSocketInfo->ai_family = AF_INET;
      m_pSocketInfo->ai_flags = AI_PASSIVE;

      // Stores concrete address.
      struct addrinfo* result;

      // Resolve the server address and port. Get a concrete address and port.
      // Note that later, the Socket will listen to the 'DEFAULT_PORT'
      // Since this addrinfo is used in "Bind" is is specifying the local port.
      m_iResult = getaddrinfo(NULL, iport, m_pSocketInfo, &result);
      if (m_iResult != 0)
      {
         printf("getaddrinfo failed with error: %d\n", m_iResult);
         WSACleanup();
         exit(1);
      }

      // Create a concrete socket object
      m_Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

      if (m_Socket == INVALID_SOCKET)
      {
         printf("socket failed with error: %ld\n", WSAGetLastError());
         freeaddrinfo(result);
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

      // Setup the TCP listening socket. Bind the socket
      m_iResult = bind(m_Socket, result->ai_addr, (int)result->ai_addrlen);
      if (m_iResult == SOCKET_ERROR)
      {
         printf("bind failed with error: %d\n", WSAGetLastError());
         freeaddrinfo(result);
         closesocket(m_Socket);
         WSACleanup();
         exit(1);
      }

      // no longer need address information
      freeaddrinfo(result);
   }
}


ServerSocket::~ServerSocket()
{
}

int ServerSocket::Begin()
{
   // start listening for new clients attempting to connect
   m_iResult = listen(m_Socket, SOMAXCONN);
   if (m_iResult == SOCKET_ERROR)
   {
      printf("listen failed with error: %d\n", WSAGetLastError());
      closesocket(m_Socket);
      WSACleanup();
      exit(1);
   }

   return m_iResult;
}

bool ServerSocket::AcceptNewConnection(unsigned int id)
{
   // if client waiting, accept the connection and save the socket
   SOCKET clientSocket = accept(m_Socket, NULL, NULL);

   if (clientSocket != INVALID_SOCKET)
   {
      //disable nagle on the client's socket
      char value = 1;
      setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

      // insert new client into session id table
      m_Sessions.insert(std::make_pair(id, clientSocket));

      return true;
   }

   return false;
}

bool ServerSocket::HasClient(unsigned int id)
{
   return m_Sessions.find(id) != m_Sessions.end();
}

std::vector<unsigned int> ServerSocket::GetClients()
{
   std::vector<unsigned int> veciRetval;

   std::map<unsigned int, SOCKET>::iterator iter_clients;
   for( iter_clients = m_Sessions.begin();
        iter_clients != m_Sessions.end();
        ++iter_clients )
   {
      veciRetval.push_back(iter_clients->first);
   }

   return veciRetval;
}

// receive incoming data
int ServerSocket::ReceiveData(unsigned int client_id, char * recvbuf, unsigned int aiBuffsize)
{
   if (m_Sessions.find(client_id) != m_Sessions.end())
   {
      int iCharsRecved = 0;
      SOCKET currentSocket = m_Sessions[client_id];
      iCharsRecved = recv(currentSocket, recvbuf, aiBuffsize, 0);

      if (iCharsRecved == 0)
      {
         closeCollection(client_id);
      }

      return iCharsRecved;
   }

   return 0;
}


// send data to all clients
void ServerSocket::SendTo(unsigned int aiClient, const std::string& aszMsg)
{
   if (m_Sessions.find(aiClient) != m_Sessions.end())
   {
      SOCKET currentSocket = m_Sessions.at(aiClient);
      int iSendResult;
      const char* cphMsg = aszMsg.c_str();
      iSendResult = send(currentSocket, cphMsg, aszMsg.size(), 0);

      if (iSendResult == SOCKET_ERROR)
      {
         printf("send failed with error: %d\n", WSAGetLastError());
         closeCollection(aiClient);
      }

      Sleep(100);
   }
}

void ServerSocket::closeCollection(unsigned int aiClient)
{
   SOCKET currentSocket = m_Sessions[aiClient];

   printf("Connection closed\n");
   closesocket(currentSocket);

   m_Sessions.erase(aiClient);
}