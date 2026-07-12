#include "ClientManager.h"
#include "Socket.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

std::mutex coutMutex;

void handleClient(
    std::shared_ptr<Socket> client, 
    ClientManager& manager)
{
    try
    {
        while (true)
        {
            std:: string message = client->receive();

            {
                std::lock_guard<std::mutex> lock(coutMutex);

                std::cout 
                    << "Received: "
                    << message
                    << "\n";
            }

            manager.broadcast(message);
        }
    }
    catch (const std::exception&)
    {
        manager.removeClient(client);

        {
            std::lock_guard<std::mutex> lock(coutMutex);

            std::cout
                << "Client disconnected.\n";
        }
    }
    
}

int main()
{
    try
    {
        Socket server;

        server.bind(8080);
        server.listen(10);

        std::cout
            << "Server listening on port 8080.\n";

        while (true)
        {
            Socket client = server.accept();

            {
                std::lock_guard<std::mutex> lock(coutMutex);

                std::cout
                    << "Client connected.\n";
            }

            std:: thread clientThread(
                handleClient,
                std::move(client));

            clientThread.detach();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr
            << "Error: "
            << e.what()
            << "\n";

        return 1;
    }

    return 0;
}