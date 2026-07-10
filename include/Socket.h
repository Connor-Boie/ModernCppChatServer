#pragma once

class Socket
{
public:
    Socket();
    ~Socket();

    int getFd() const;

private:
    int m_fd;
};