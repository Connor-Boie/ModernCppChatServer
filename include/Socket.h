#pragma once

class Socket
{
public:
    Socket();
    ~Socket();

    int getFd() const;

    void bind(int port);
    void listen(int backlog);

private:
    int m_fd;
};