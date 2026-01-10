#ifndef FIX_DCOPY_SOCKET_H
#define FIX_DCOPY_SOCKET_H

#include <cstdint>
#include <string>
#include <vector>

class Socket {
public:
    Socket();
    ~Socket();

    bool connect_to_server(const std::string& server_ip, std::uint16_t server_port);
    void close_socket();

    bool send_bytes(const std::string& data);
    int recv_bytes(std::vector<char>& buffer, std::size_t max_bytes);

    bool is_connected() const;
    const std::string& last_error() const;

private:
    int socket_fd;
};

#endif
