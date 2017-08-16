// CollectionServer.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <vector>
#include "Server.h"
#include "ClientSocket.h"

int comm(Server& server)
{
   std::vector<unsigned int> veciClients;
   std::string sziClient;
   unsigned int iClient;
   bool bExit = false;
   bool bHasCon = false;

   std::cout << "Awaiting Connections...";
   while (!bExit)
   {
      if (bHasCon)
      {
         std::cout << "Available Collections";
         for (size_t i = 0; i < veciClients.size(); i++)
         {
            std::cout << ", ";
            std::cout << std::to_string(i);
         }
         std::cout << std::endl;

         std::cout << "Send Msg To: ";
         std::cin >> sziClient;
         std::cout << std::endl;

         iClient = std::stoi(sziClient);

         std::cout << "Send Msg: ";
         std::cin >> sziClient;
         std::cout << std::endl;

         server.SendMsg(iClient, sziClient);

         veciClients = server.GetClients();
         bHasCon = veciClients.size() > 0;
      }
      else
      {
         veciClients = server.GetClients();
         bHasCon = veciClients.size() > 0;
      }

      Sleep(200);
   }

   return 0;
}

int test(Server& server)
{
   std::string rszInput;
   bool bExit;
   ClientSocket client;

   server.Start(true);
   client.BeginConnect();

   std::cout << "Send Msg: ";
   std::cin >> rszInput;
   std::cout << std::endl;

   bExit = rszInput == "quit";
   
   while (!bExit)
   {
      char recvBuf[PACKET_SIZE];

      client.SendData(rszInput);
      Sleep(200);
      int iChars = client.ReceiveData(recvBuf, PACKET_SIZE);

      if (iChars > 0)
      {
         std::cout << "Resp: ";
         std::cout << std::string(recvBuf, recvBuf + iChars);
         std::cout << std::endl;
      }
      else
      {
         std::cout << "No Response" << std::endl;
      }

      std::cout << "Send Msg: ";
      std::cin >> rszInput;
      std::cout << std::endl;

      bExit = rszInput == "quit";
   }
   return 0;
}

int echo(Server& server)
{
   server.Start(true);
   while(true){}
   return 0;
}

int main()
{
   Server server;
   std::string szInput;
   bool bExit = false;

   std::cout << "Choose Mode (Echo, Comm, Test): ";
   std::cin >> szInput;
   std::cout << std::endl;

   server.Start(szInput == "Echo" || szInput == "Test");

   if (szInput == "Test")
   {
      test(server);
   }
   else if (szInput == "Comm")
   {
      comm(server);
   }
   else
   {
      echo(server);
   }

   return 0;
}
