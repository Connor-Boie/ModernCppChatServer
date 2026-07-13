#include "Socket.h"

#include <array>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

namespace
{
std::runtime_error socketError(
    const std::string& operation)
{
    return std::runtime_error(
        operation + ": " + std::strerror(errno));
}
}

Socket::Socket()
    : m_fd(::socket(
          AF_INET,
          SOCK_STREAM,
          IPPROTO_TCP))
{
    if (m_fd == -1)
    {
        throw socketError(
            "Failed to create socket");
    }

    const int reuseAddress = 1;

    if (::setsockopt(
            m_fd,
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuseAddress,
            sizeof(reuseAddress)) == -1)
    {
        const auto error =
            socketError(
                "Failed to configure socket");

        ::close(m_fd);
        m_fd = -1;

        throw error;
    }
}

Socket::Socket(int fd)
    : m_fd(fd)
{
    if (m_fd == -1)
    {
        throw std::invalid_argument(
            "Cannot construct Socket from an "
            "invalid file descriptor");
    }
}

Socket::Socket(Socket&& other) noexcept
    : m_fd(other.m_fd)
{
    other.m_fd = -1;
}

Socket& Socket::operator=(
    Socket&& other) noexcept
{
    if (this != &other)
    {
        if (m_fd != -1)
        {
            ::close(m_fd);
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
        ::close(m_fd);
    }
}

int Socket::getFd() const noexcept
{
    return m_fd;
}

void Socket::bind(int port)
{
    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr =
        htonl(INADDR_ANY);

    address.sin_port =
        htons(static_cast<uint16_t>(port));

    if (::bind(
            m_fd,
            reinterpret_cast<
                const sockaddr*>(&address),
            sizeof(address)) == -1)
    {
        throw socketError(
            "Failed to bind socket");
    }
}

void Socket::listen(int backlog)
{
    if (::listen(m_fd, backlog) == -1)
    {
        throw socketError(
            "Failed to listen on socket");
    }
}

Socket Socket::accept()
{
    const int clientFd =
        ::accept(
            m_fd,
            nullptr,
            nullptr);

    if (clientFd == -1)
    {
        throw socketError(
            "Failed to accept client");
    }

    return Socket(clientFd);
}

std::string Socket::receive()
{
    std::array<char, 1024> buffer{};

    const ssize_t bytesReceived =
        ::recv(
            m_fd,
            buffer.data(),
            buffer.size(),
            0);

    if (bytesReceived == -1)
    {
        throw socketError(
            "Failed to receive data");
    }

    if (bytesReceived == 0)
    {
        return {};
    }

    return std::string(
        buffer.data(),
        static_cast<std::size_t>(
            bytesReceived));
}

void Socket::send(
    const std::string& message)
{
    std::size_t totalSent = 0;

    while (totalSent < message.size())
    {
        const ssize_t bytesSent =
            ::send(
                m_fd,
                message.data() + totalSent,
                message.size() - totalSent,
                MSG_NOSIGNAL);

        if (bytesSent == -1)
        {
            throw socketError(
                "Failed to send data");
        }

        totalSent +=
            static_cast<std::size_t>(
                bytesSent);
    }
}

void Socket::shutdown() noexcept
{
    if (m_fd != -1)
    {
        ::shutdown(m_fd, SHUT_RDWR);
    }
}