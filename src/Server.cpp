#include "Server.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

namespace
{
void removeLineEnding(std::string& text)
{
    while (
        !text.empty()
        && (text.back() == '\n' || text.back() == '\r'))
    {
        text.pop_back();
    }
}
}

Server::Server(int port, int backlog)
    : m_port(port)
{
    m_listener.bind(m_port);
    m_listener.listen(backlog);
}

void Server::run()
{
    log(
        "Server listening on port "
        + std::to_string(m_port)
        + ".");

    while (true)
    {
        auto client =
            std::make_shared<Socket>(
                m_listener.accept());

        addClient(client);

        log(
            "Client connected. File descriptor: "
            + std::to_string(client->getFd()));

        std::thread clientThread(
            &Server::handleClient,
            this,
            client);

        clientThread.detach();
    }
}

void Server::handleClient(
    std::shared_ptr<Socket> client)
{
    const int clientFd = client->getFd();
    std::string username;

    try
    {
        client->send("Enter your username: ");

        username = client->receive();
        removeLineEnding(username);

        if (username.empty())
        {
            username =
                "Guest"
                + std::to_string(clientFd);
        }

        client->send(
            "Welcome, "
            + username
            + "!\n");

        log(
            username
            + " joined the chat. File descriptor: "
            + std::to_string(clientFd));

        broadcast(
            "*** "
            + username
            + " joined the chat. ***\n",
            clientFd);

        while (true)
        {
            std::string message =
                client->receive();

            if (message.empty())
            {
                break;
            }

            removeLineEnding(message);

            if (message.empty())
            {
                continue;
            }

            log(
                "Received from "
                + username
                + ": "
                + message);

            broadcast(
                username
                + ": "
                + message
                + "\n",
                clientFd);

            client->send("Message sent.\n");
        }
    }
    catch (const std::exception& error)
    {
        log(
            "Client "
            + std::to_string(clientFd)
            + " error: "
            + error.what());
    }

    if (!username.empty())
    {
        broadcast(
            "*** "
            + username
            + " left the chat. ***\n",
            clientFd);

        log(username + " left the chat.");
    }
    else
    {
        log(
            "Client disconnected before choosing a username. "
            "File descriptor: "
            + std::to_string(clientFd));
    }

    removeClient(clientFd);
}

void Server::addClient(
    const std::shared_ptr<Socket>& client)
{
    std::lock_guard<std::mutex> lock(
        m_clientsMutex);

    m_clients.push_back(client);
}

void Server::removeClient(int clientFd)
{
    std::lock_guard<std::mutex> lock(
        m_clientsMutex);

    m_clients.erase(
        std::remove_if(
            m_clients.begin(),
            m_clients.end(),
            [clientFd](
                const std::shared_ptr<Socket>& client)
            {
                return client->getFd() == clientFd;
            }),
        m_clients.end());
}

void Server::broadcast(
    const std::string& message,
    int senderFd)
{
    std::lock_guard<std::mutex> lock(
        m_clientsMutex);

    for (const auto& client : m_clients)
    {
        if (client->getFd() == senderFd)
        {
            continue;
        }

        try
        {
            client->send(message);
        }
        catch (const std::exception& error)
        {
            log(
                "Broadcast to client "
                + std::to_string(client->getFd())
                + " failed: "
                + error.what());
        }
    }
}

void Server::log(const std::string& message)
{
    std::lock_guard<std::mutex> lock(
        m_outputMutex);

    std::cout << message;

    if (message.empty() || message.back() != '\n')
    {
        std::cout << '\n';
    }
}