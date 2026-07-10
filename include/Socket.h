#pragma once

class Socket
{
public:
    Socket();
    ~Socket();

    int getFd() const;

    void bind(int port);

private:
    int m_fd;
};