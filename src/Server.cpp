#include "Server.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>
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
    bool usernameRegistered = false;

    try
    {
        while (!usernameRegistered)
        {
            client->send("Enter your username: ");

            username = client->receive();

            if (username.empty())
            {
                break;
            }

            removeLineEnding(username);

            if (username.empty())
            {
                client->send(
                    "Username cannot be empty.\n");

                continue;
            }

            usernameRegistered =
                tryRegisterUsername(username);

            if (!usernameRegistered)
            {
                client->send(
                    "That username is already in use. "
                    "Please choose another.\n");
            }
        }

        if (usernameRegistered)
        {
            client->send(
                "Welcome, "
                + username
                + "!\n");

            client->send(
                "Type /help to view available commands.\n");

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

                if (message == "/quit")
                {
                    client->send("Goodbye!\n");
                    break;
                }

                if (message.front() == '/')
                {
                    handleCommand(message, client);
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
    }
    catch (const std::exception& error)
    {
        log(
            "Client "
            + std::to_string(clientFd)
            + " error: "
            + error.what());
    }

    if (usernameRegistered)
    {
        unregisterUsername(username);

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

bool Server::tryRegisterUsername(
    const std::string& username)
{
    std::lock_guard<std::mutex> lock(
        m_usernamesMutex);

    const bool inserted =
        m_usernames.insert(username).second;

    return inserted;
}

void Server::unregisterUsername(
    const std::string& username)
{
    std::lock_guard<std::mutex> lock(
        m_usernamesMutex);

    m_usernames.erase(username);
}

void Server::handleCommand(
    const std::string& command,
    const std::shared_ptr<Socket>& client)
{
    if (command == "/help")
    {
        client->send(
            "Available commands:\n"
            "  /help  - Show available commands\n"
            "  /users - Show connected users\n"
            "  /quit  - Leave the chat\n");

        return;
    }

    if (command == "/users")
    {
        client->send(buildUserList());
        return;
    }

    client->send(
        "Unknown command: "
        + command
        + "\n"
        + "Type /help to view available commands.\n");
}

std::string Server::buildUserList()
{
    std::lock_guard<std::mutex> lock(
        m_usernamesMutex);

    std::ostringstream output;

    output
        << "Connected users ("
        << m_usernames.size()
        << "):\n";

    for (const auto& username : m_usernames)
    {
        output << "  - " << username << '\n';
    }

    return output.str();
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