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

void Socket::bind(int port)
{
    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (::bind(
        m_fd,
        reinterpret_cast<sockaddr*>(&address),
        sizeof(address)) == -1)
    {
        throw std::runtime_error("Failed to bind socket");
    }
}