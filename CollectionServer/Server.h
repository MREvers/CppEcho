#pragma once
#include <list>
#include <queue>
#include <string>
#include <vector>
#include <mutex>
#include "ServerSocket.h"

class Server
{
public:
   Server();
   ~Server();

   void Start(bool abEcho = false);
   void Stop();

   void ServerThread();
   std::vector<unsigned int> GetClients();
   void SendMsg(unsigned int aiClient, std::string aszMsg);
   std::string Echo(unsigned int aiClient);

private:
   ServerSocket* m_ServerSocket;
   std::mutex m_MsgLock;

   bool m_bRunning;
   bool m_bEcho;
   unsigned int m_iClientID;
   std::queue<std::pair<unsigned int, std::string>> m_queMsgs;
};

