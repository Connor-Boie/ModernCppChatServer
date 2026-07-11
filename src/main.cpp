#include "Socket.h"

#include <iostream>
#include <thread>

void handleClient(Socket client)
{
    try
    {
        while (true)
        {
            std:: string message = client.receive();

            std::cout 
                << "Received: "
                << message
                << "\n";

            client.send("Message received!\n");
        }
    }
    catch(...)
    {
        std::cout
            << "Client disconnected.\n";
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

            std::cout
                << "Client connected.\n";

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