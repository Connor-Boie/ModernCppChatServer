#pragma once

#include <string>

class Socket
{
public:
    Socket();
    explicit Socket(int fd);
    ~Socket();

    int getFd() const;

    void bind(int port);
    void listen(int backlog);

    Socket accept();

    std::string receive();
    void send(const std::string& message);

private:
    int m_fd;
};