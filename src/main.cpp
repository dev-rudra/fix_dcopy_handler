#include "socket.h"
#include <iostream>

int main() {
    socket sock;

    if (!sock.connect_to_server("127.0.0.1", 9000)) {
        std::cout << "connect failed\n";
        return 1;
    }

    std::cout << "connected\n";

    sock.send_bytes("hello from socket\n");

    std::vector<char> buf;
    int n = sock.recv_bytes(buf, 1024);
    if (n > 0) {
        std::cout << "received: " << std::string(buf.begin(), buf.end());
    }

    return 0;
}
