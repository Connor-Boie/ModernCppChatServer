#pragma once

#include "Socket.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class LogLevel
{
    Info,
    Warning,
    Error
};

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

    [[nodiscard]] bool tryRegisterUsername(
        const std::string& username,
        const std::shared_ptr<Socket>& client);

    void unregisterUsername(
        const std::string& username);

    void handleCommand(
        const std::string& command,
        const std::string& senderUsername,
        const std::shared_ptr<Socket>& senderClient);

    void sendPrivateMessage(
        const std::string& senderUsername,
        const std::string& recipientUsername,
        const std::string& message,
        const std::shared_ptr<Socket>& senderClient);

    [[nodiscard]] std::string buildUserList();

    void broadcast(
        const std::string& message,
        int senderFd);

    void log(
        LogLevel level,
        const std::string& message);

    Socket m_listener;
    int m_port;

    std::vector<std::shared_ptr<Socket>> m_clients;

    std::unordered_set<std::string> m_usernames;

    std::unordered_map<
        std::string,
        std::shared_ptr<Socket>> m_userSockets;

    std::mutex m_clientsMutex;
    std::mutex m_usernamesMutex;
    std::mutex m_outputMutex;
};