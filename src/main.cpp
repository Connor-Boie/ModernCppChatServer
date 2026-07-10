#include "Socket.h"

#include <iostream>

int main()
{
    try
    {
        Socket socket;

        socket.bind(8080);

        std::cout
            << "Server bound to port 8080.\n";
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