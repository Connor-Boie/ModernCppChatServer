#include "Server.h"

#include <exception>
#include <iostream>

int main()
{
    try
    {
        Server server(8080);
        server.run();
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Server error: "
            << error.what()
            << '\n';

        return 1;
    }

    return 0;
}