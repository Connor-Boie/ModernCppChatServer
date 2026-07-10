#include "Socket.h"

#include <unistd.h>

Socket::Socket()
    : m_fd(-1)
{
}

Socket::~Socket()
{
    if (m_fd != -1)
    {
        close(m_fd);
    }
}

int Socket::getFd() const
{
    return m_fd;
}