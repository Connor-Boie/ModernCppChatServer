#pragma once

#include "Socket.h"

#include <memory>
#include <mutex>
#include <vector>

class Server
{
public:
    explicit Server(int port, int backlog = 10);

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    void run();

private:
    void handleClient(std::shared_ptr<Socket> client);

    void addClient(const std::shared_ptr<Socket>& client);
    void removeClient(int clientFd);

    void broadcast(
        const std::string& message,
        int senderFd);

    void log(const std::string& message);

    Socket m_listener;
    int m_port;

    std::vector<std::shared_ptr<Socket>> m_clients;

    std::mutex m_clientsMutex;
    std::mutex m_outputMutex;
};