#include "ClientManager.h"

#include <algorithm>

void ClientManager::addClient(std::shared_ptr<Socket> client)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_clients.push_back(client);
}

void ClientManager::removeClient(std::shared_ptr<Socket> client)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_clients.erase(
        std::remove(
            m_clients.begin(),
            m_clients.end(),
            client),
        m_clients.end());
}

std::vector<std::shared_ptr<Socket>> ClientManager::getClients()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_clients;
}