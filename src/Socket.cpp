#include "Socket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <unistd.h>

Socket::Socket()
{
    m_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_fd == -1)
    {
        throw std::runtime_error("Failed to create socket");
    }
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