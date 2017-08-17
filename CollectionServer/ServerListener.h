#pragma once
#include <mutex>
#include "Server.h"


class ServerListener
{
public:
   ServerListener();
   ~ServerListener();

   int Listen(Server* server, std::mutex* aIOMutex, bool* bControl);

private:
   int listen();

   Server* m_pServer;
   std::mutex* m_pIOMutex;
   bool* m_bControl;
};

