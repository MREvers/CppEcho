#include "Server.h"
#include <process.h>
#include <iostream>


Server::Server()
{
   m_iClientID = 0;
   m_bEcho = false;

   // Start the port listening right away.
   m_ServerSocket = new ServerSocket();
   m_ServerSocket->Begin();
}


Server::~Server()
{
   delete m_ServerSocket;
}

void Server::Start(bool abEcho)
{
   m_bRunning = true;
   m_bEcho = abEcho;

   std::thread t(&Server::ServerThread, this);
   t.detach();
}

void Server::Stop()
{
   m_bRunning = false;
}


bool Server::HasClient(unsigned int aiClient)
{
   return m_ServerSocket->HasClient(aiClient);
}


void Server::ServerThread()
{
   while (m_bRunning)
   {
      bool bNewClient = m_ServerSocket->AcceptNewConnection(m_iClientID);
      m_iClientID += bNewClient;

      std::vector<unsigned int> veciClients = m_ServerSocket->GetClients();
      std::vector<unsigned int>::iterator iter_clients;
      for ( iter_clients = veciClients.begin();
            iter_clients != veciClients.end();
            ++iter_clients )
      {
         std::string szRecved;

         // Do response stuff here.
         if (m_bEcho)
         {
            szRecved = Echo( *iter_clients );
         }

         if (szRecved.size() > 0)
         {
            std::cout << "*Recved (";
            std::cout << std::to_string(*iter_clients) + "): ";
            std::cout << szRecved << std::endl;
         }
      }

      // Send messages in the message queue.
      m_MsgLock.lock();
      while (m_queMsgs.size() > 0)
      {
         m_ServerSocket->SendTo( m_queMsgs.front().first, 
                                 m_queMsgs.front().second );
         m_queMsgs.pop();
      }
      m_MsgLock.unlock();
   }
}


std::vector<unsigned int> Server::GetClients()
{
   return m_ServerSocket->GetClients();
}


void Server::SendMsg(unsigned int aiClient, std::string aszMsg)
{
   m_MsgLock.lock();
   m_queMsgs.push(std::make_pair(aiClient, aszMsg));
   m_MsgLock.unlock();
}


std::string Server::RecvMsg(unsigned int aiClient)
{
   char recvBuf[PACKET_SIZE];
   std::string szEcho;
   int iRespSize = m_ServerSocket->ReceiveData( aiClient, recvBuf, PACKET_SIZE );

   if (iRespSize > 0)
   {
      szEcho = std::string(recvBuf, recvBuf + iRespSize);
   }

   return szEcho;
}


std::string Server::Echo(unsigned int aiClient)
{
   char recvBuf[PACKET_SIZE];
   std::string szEcho;
   int iRespSize = m_ServerSocket->ReceiveData( aiClient, recvBuf, PACKET_SIZE );

   if (iRespSize > 0)
   {
      szEcho = std::string(recvBuf, recvBuf + iRespSize);
      m_ServerSocket->SendTo( aiClient, szEcho );
   }

   return szEcho;
}