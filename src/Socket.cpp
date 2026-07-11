#include "Socket.h"

#include <array>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

Socket::Socket()
{
    m_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_fd == -1)
    {
        throw std::runtime_error("Failed to create socket");
    }
}

Socket::Socket(int fd)
    : m_fd(fd)
{
}

Socket::Socket(Socket&& other) noexcept
    : m_fd(other.m_fd)

{
    other.m_fd = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept
{
    if (this != &other)
    {
        if (m_fd != -1)
        {
            close(m_fd);
        }

        m_fd = other.m_fd;
        other.m_fd = -1;
    }

    return *this;
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
        throw std::runtime_error("Failed to bind socket.");
    }
}

void Socket::listen(int backlog)
{
    if (::listen(m_fd, backlog) == -1)
    {
        throw std::runtime_error("Failed to listen on socket.");
    }
}

Socket Socket::accept()
{
    int client_fd = ::accept(
        m_fd,
        nullptr,
        nullptr
    );

    if (client_fd == -1)
    {
        throw std::runtime_error("Failed to accept client.");
    }

    // Wrap the accepted file descriptor in a Socket object so the
    // connection is automatically closed when the object is destroyed.
    return Socket(client_fd);
}

std::string Socket::receive()
{
    std::array<char, 1024> buffer{};

    ssize_t bytesReceived = 
        recv(
            m_fd,
            buffer.data(),
            buffer.size(),
            0);
    
    if (bytesReceived == -1)
    {
        throw std::runtime_error("Failed to receive data.");
    }

    return std::string(
        buffer.data(),
        bytesReceived);
}

void Socket::send(const std::string& message)
{
    ssize_t bytesSent = 
        ::send(
            m_fd,
            message.c_str(),
            message.size(),
            0);
        
    if (bytesSent == -1)
    {
        throw std::runtime_error("Failed to send data.");
    }
}