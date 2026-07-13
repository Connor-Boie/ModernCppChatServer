#pragma once

#include <string>

class Socket
{
public:
    Socket();
    explicit Socket(int fd);

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    ~Socket();

    [[nodiscard]] int getFd() const noexcept;

    void bind(int port);
    void listen(int backlog);
    [[nodiscard]] Socket accept();

    [[nodiscard]] std::string receive();
    void send(const std::string& message);

    void shutdown() noexcept;

private:
    int m_fd{-1};
};