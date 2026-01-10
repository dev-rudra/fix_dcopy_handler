#include "socket.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket()
    : socket_fd(-1) {}

Socket::~Socket() {
    close_socket();
}

bool Socket::connect_to_server(const std::string& server_ip, std::uint16_t server_port) {
    close_socket();

    socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        return false;
    }

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);

    if (::inet_pton(AF_INET, server_ip.c_str(), &addr.sin_addr) != 1) {
        close_socket();
        return false;
    }

    if (::connect(socket_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) <0) {
        close_socket();
        return false;
    }

    return true;
}

void Socket::close_socket() {
    if (socket_fd >= 0) {
        ::close(socket_fd);
        socket_fd = -1;
    }
}

bool Socket::send_bytes(const std::string& data) {
    if (socket_fd < 0) {
        return false;
    }

    const char* buffer_ptr = data.data();
    std::size_t bytes_remaining = data.size();

    while (bytes_remaining > 0) {
        const ssize_t bytes_sent = ::send(socket_fd, buffer_ptr, bytes_remaining, 0);
        if (bytes_sent > 0) {
            buffer_ptr += bytes_sent;
            bytes_remaining -= static_cast<std::size_t>(bytes_sent);
            continue;
        }

        if (bytes_sent < 0 && errno == EINTR) {
            continue;
        }

        return false;
    }

    return true;
}

int Socket::recv_bytes(std::vector<char>& buffer, std::size_t max_bytes) {
    if (socket_fd < 0) {
        return -1;
    }

    buffer.resize(max_bytes);

    const ssize_t bytes_received = ::recv(socket_fd, buffer.data(), max_bytes, 0);
    if (bytes_received > 0) {
        buffer.resize(static_cast<std::size_t>(bytes_received));
        return static_cast<int>(bytes_received);
    }

    if (bytes_received == 0) {
        buffer.clear();
        return 0;
    }

    if (errno == EINTR) {
        return -1;
    }

    return -1;
}

bool Socket::is_connected() const {
    return socket_fd >= 0;
}
