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

        int client_fd = socket.accept();

        std::cout
            << "Client connected! fd: "
            << client_fd
            << "\n";
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