#pragma once

#include "Socket.h"

#include <memory>
#include <mutex>
#include <vector>

class ClientManager
{
public:
    void addClient(std::shared_ptr<Socket> client);

    void removeClient(std::shared_ptr<Socket> client);

    std::vector<std::shared_ptr<Socket>> getClients();

private:
    std::vector<std::shared_ptr<Socket>> m_clients;

    std::mutex m_mutex;
};