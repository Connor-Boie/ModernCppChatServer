#include "Socket.h"

#include <iostream>

int main()
{
    try
    {
        Socket socket;

        std::cout
            << "Socket created with fd: "
            << socket.getFd()
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