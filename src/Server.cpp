#include "Server.h"

#include <exception>
#include <iostream>
#include <string>
#include <thread>
#include <utility>

Server::Server(int port, int backlog)
    : m_port(port)
{
    m_listener.bind(m_port);
    m_listener.listen(backlog);
}

void Server::run()
{
    {
        std::lock_guard<std::mutex> lock(m_outputMutex);

        std::cout
            << "Server listening on port "
            << m_port
            << ".\n";
    }

    while (true)
    {
        Socket client = m_listener.accept();

        log("Client connected.");

        std::thread clientThread(
            &Server::handleClient,
            this,
            std::move(client));

        clientThread.detach();
    }
}

void Server::handleClient(Socket client)
{
    try
    {
        while (true)
        {
            const std::string message = client.receive();

            if (message.empty())
            {
                break;
            }

            {
                std::lock_guard<std::mutex> lock(m_outputMutex);

                std::cout
                    << "Received: "
                    << message;

                if (message.back() != '\n')
                {
                    std::cout << '\n';
                }
            }

            client.send("Message received!\n");
        }
    }
    catch (const std::exception& error)
    {
        std::lock_guard<std::mutex> lock(m_outputMutex);

        std::cerr
            << "Client error: "
            << error.what()
            << '\n';
    }

    log("Client disconnected.");
}

void Server::log(const char* message)
{
    std::lock_guard<std::mutex> lock(m_outputMutex);

    std::cout << message << '\n';
}