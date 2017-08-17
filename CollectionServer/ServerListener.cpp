#include "ServerListener.h"
#include <vector>
#include <iostream>
#include <thread>

ServerListener::ServerListener()
{
}


ServerListener::~ServerListener()
{
}


int ServerListener::Listen(Server* server, std::mutex* aIOMutex, bool* bControl)
{
   m_pServer = server;
   m_pIOMutex = aIOMutex;
   m_bControl = bControl;

   std::thread t(&ServerListener::listen, this);
   t.detach();

   return 0;
}

int ServerListener::listen()
{
   std::vector<unsigned int> veciClients;

   while (*m_bControl)
   {
      veciClients = m_pServer->GetClients();

      std::vector<unsigned int>::iterator iter_client;
      for (iter_client = veciClients.begin();
         iter_client != veciClients.end();
         ++iter_client)
      {
         std::string szResp;

         szResp = m_pServer->RecvMsg(*iter_client);
         if (szResp.size() > 0)
         {
            m_pIOMutex->lock();
            std::cout << "Recv: (" << *iter_client << ") " << szResp << std::endl;
            m_pIOMutex->unlock();
         }
      }
   }

   return 0;
}