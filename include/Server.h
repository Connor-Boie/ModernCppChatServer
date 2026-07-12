#pragma once

#include "Socket.h"

#include <mutex>

class Server
{
public:
    explicit Server(int port, int backlog = 10);

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    void run();

private:
    void handleClient(Socket client);
    void log(const char* message);

    Socket m_listener;
    int m_port;
    std::mutex m_outputMutex;
};