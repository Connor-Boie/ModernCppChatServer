#include "Server.h"

#include <algorithm>
#include <chrono>
#include <csignal>
#include <ctime>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace
{
volatile std::sig_atomic_t shutdownRequested = 0;

void requestShutdown(int signalNumber)
{
    if (signalNumber == SIGINT)
    {
        shutdownRequested = 1;
    }
}

void installSignalHandler()
{
    struct sigaction action
    {
    };

    action.sa_handler = requestShutdown;

    sigemptyset(
        &action.sa_mask);

    action.sa_flags = 0;

    if (sigaction(
            SIGINT,
            &action,
            nullptr) == -1)
    {
        throw std::runtime_error(
            "Failed to install SIGINT handler");
    }
}

void removeLineEnding(
    std::string& text)
{
    while (
        !text.empty()
        && (
            text.back() == '\n'
            || text.back() == '\r'))
    {
        text.pop_back();
    }
}

std::string logLevelToString(
    LogLevel level)
{
    switch (level)
    {
        case LogLevel::Info:
            return "INFO";

        case LogLevel::Warning:
            return "WARNING";

        case LogLevel::Error:
            return "ERROR";
    }

    return "UNKNOWN";
}

std::string currentTimestamp()
{
    const auto now =
        std::chrono::system_clock::now();

    const std::time_t currentTime =
        std::chrono::system_clock::
            to_time_t(now);

    std::tm localTime{};

    localtime_r(
        &currentTime,
        &localTime);

    std::ostringstream output;

    output << std::put_time(
        &localTime,
        "%Y-%m-%d %H:%M:%S");

    return output.str();
}
}

Server::Server(
    int port,
    int backlog)
    : m_port(port)
{
    m_listener.bind(m_port);
    m_listener.listen(backlog);
}

void Server::run()
{
    shutdownRequested = 0;

    installSignalHandler();

    log(
        LogLevel::Info,
        "Server listening on port "
        + std::to_string(m_port)
        + ".");

    log(
        LogLevel::Info,
        "Press Ctrl+C to stop the server.");

    while (!shutdownRequested)
    {
        try
        {
            auto client =
                std::make_shared<Socket>(
                    m_listener.accept());

            addClient(client);

            log(
                LogLevel::Info,
                "Client connected. "
                "File descriptor: "
                + std::to_string(
                    client->getFd()));

            m_clientThreads.emplace_back(
                &Server::handleClient,
                this,
                client);
        }
        catch (const std::exception&)
        {
            if (shutdownRequested)
            {
                break;
            }

            throw;
        }
    }

    log(
        LogLevel::Info,
        "Shutdown requested. "
        "Stopping client connections.");

    shutdownClients();
    joinClientThreads();

    log(
        LogLevel::Info,
        "All client threads stopped. "
        "Server shutdown complete.");
}

void Server::handleClient(
    std::shared_ptr<Socket> client)
{
    const int clientFd =
        client->getFd();

    std::string username;
    bool usernameRegistered = false;

    try
    {
        while (!usernameRegistered)
        {
            client->send(
                "Enter your username: ");

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
                tryRegisterUsername(
                    username,
                    client);

            if (!usernameRegistered)
            {
                client->send(
                    "That username is already "
                    "in use. Please choose "
                    "another.\n");
            }
        }

        if (usernameRegistered)
        {
            client->send(
                "Welcome, "
                + username
                + "!\n");

            client->send(
                "Type /help to view "
                "available commands.\n");

            log(
                LogLevel::Info,
                username
                + " joined the chat. "
                "File descriptor: "
                + std::to_string(clientFd));

            broadcast(
                "*** "
                + username
                + " joined the chat. ***\n",
                clientFd);

            while (!shutdownRequested)
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
                    handleCommand(
                        message,
                        username,
                        client);

                    continue;
                }

                log(
                    LogLevel::Info,
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

                client->send(
                    "Message sent.\n");
            }
        }
    }
    catch (const std::exception& error)
    {
        if (!shutdownRequested)
        {
            log(
                LogLevel::Error,
                "Client "
                + std::to_string(clientFd)
                + " error: "
                + error.what());
        }
    }

    if (usernameRegistered)
    {
        unregisterUsername(username);

        if (!shutdownRequested)
        {
            broadcast(
                "*** "
                + username
                + " left the chat. ***\n",
                clientFd);
        }

        log(
            LogLevel::Info,
            username + " left the chat.");
    }
    else if (!shutdownRequested)
    {
        log(
            LogLevel::Warning,
            "Client disconnected before "
            "choosing a username. "
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
                const std::shared_ptr<
                    Socket>& client)
            {
                return client->getFd()
                    == clientFd;
            }),
        m_clients.end());
}

bool Server::tryRegisterUsername(
    const std::string& username,
    const std::shared_ptr<Socket>& client)
{
    std::lock_guard<std::mutex> lock(
        m_usernamesMutex);

    const bool inserted =
        m_usernames.insert(
            username).second;

    if (!inserted)
    {
        return false;
    }

    m_userSockets[username] = client;

    return true;
}

void Server::unregisterUsername(
    const std::string& username)
{
    std::lock_guard<std::mutex> lock(
        m_usernamesMutex);

    m_usernames.erase(username);
    m_userSockets.erase(username);
}

void Server::handleCommand(
    const std::string& command,
    const std::string& senderUsername,
    const std::shared_ptr<Socket>& senderClient)
{
    if (command == "/help")
    {
        senderClient->send(
            "Available commands:\n"
            "  /help                  "
            "- Show available commands\n"
            "  /users                 "
            "- Show connected users\n"
            "  /msg <user> <message>  "
            "- Send a private message\n"
            "  /quit                  "
            "- Leave the chat\n");

        return;
    }

    if (command == "/users")
    {
        senderClient->send(
            buildUserList());

        return;
    }

    if (command.rfind("/msg", 0) == 0)
    {
        std::istringstream commandStream(
            command);

        std::string commandName;
        std::string recipientUsername;
        std::string privateMessage;

        commandStream
            >> commandName
            >> recipientUsername;

        std::getline(
            commandStream,
            privateMessage);

        if (
            !privateMessage.empty()
            && privateMessage.front() == ' ')
        {
            privateMessage.erase(0, 1);
        }

        if (
            recipientUsername.empty()
            || privateMessage.empty())
        {
            senderClient->send(
                "Usage: /msg <username> "
                "<message>\n");

            return;
        }

        sendPrivateMessage(
            senderUsername,
            recipientUsername,
            privateMessage,
            senderClient);

        return;
    }

    senderClient->send(
        "Unknown command: "
        + command
        + "\n"
        + "Type /help to view "
        "available commands.\n");
}

void Server::sendPrivateMessage(
    const std::string& senderUsername,
    const std::string& recipientUsername,
    const std::string& message,
    const std::shared_ptr<Socket>& senderClient)
{
    std::shared_ptr<Socket> recipientClient;

    {
        std::lock_guard<std::mutex> lock(
            m_usernamesMutex);

        const auto recipient =
            m_userSockets.find(
                recipientUsername);

        if (recipient == m_userSockets.end())
        {
            senderClient->send(
                "User not found: "
                + recipientUsername
                + "\n");

            return;
        }

        recipientClient =
            recipient->second;
    }

    try
    {
        recipientClient->send(
            "[Private from "
            + senderUsername
            + "] "
            + message
            + "\n");

        senderClient->send(
            "[Private to "
            + recipientUsername
            + "] "
            + message
            + "\n");

        log(
            LogLevel::Info,
            senderUsername
            + " sent a private message to "
            + recipientUsername
            + ".");
    }
    catch (const std::exception& error)
    {
        log(
            LogLevel::Error,
            "Private message delivery "
            "failed: "
            + std::string(error.what()));

        senderClient->send(
            "Could not deliver the "
            "private message.\n");
    }
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
        output
            << "  - "
            << username
            << '\n';
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
            if (!shutdownRequested)
            {
                log(
                    LogLevel::Warning,
                    "Broadcast to client "
                    + std::to_string(
                        client->getFd())
                    + " failed: "
                    + error.what());
            }
        }
    }
}

void Server::shutdownClients()
{
    std::vector<
        std::shared_ptr<Socket>> clients;

    {
        std::lock_guard<std::mutex> lock(
            m_clientsMutex);

        clients = m_clients;
    }

    for (const auto& client : clients)
    {
        client->shutdown();
    }
}

void Server::joinClientThreads()
{
    for (auto& clientThread : m_clientThreads)
    {
        if (clientThread.joinable())
        {
            clientThread.join();
        }
    }
}

void Server::log(
    LogLevel level,
    const std::string& message)
{
    std::lock_guard<std::mutex> lock(
        m_outputMutex);

    std::ostream& output =
        level == LogLevel::Error
            ? std::cerr
            : std::cout;

    output
        << '['
        << currentTimestamp()
        << "] ["
        << logLevelToString(level)
        << "] "
        << message;

    if (
        message.empty()
        || message.back() != '\n')
    {
        output << '\n';
    }
}