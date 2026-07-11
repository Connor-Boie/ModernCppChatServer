#include "Socket.h"

#include <iostream>

int main()
{
    try
    {
        Socket socket;

        socket.bind(8080);
        socket.listen(10);

        std::cout
            << "Server listening on port 8080.\n";

        Socket client = socket.accept();

        std::cout
            << "Client connected!\n";

        std::string message = client.receive();

        std::cout
            << "Received: "
            << message
            << "\n";

        client.send("Message received!\n");

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