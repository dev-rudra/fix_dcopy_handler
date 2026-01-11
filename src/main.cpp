#include <iostream>
#include <string>

#include "config_parser.h"
#include "socket.h"

int main() {
    ConfigParser cfg;
    if (!cfg.read("cfg/config.ini")) {
        std::cerr << "failed to open cfg/config.ini\n";
        return 1;
    }

    std::cout << "[dropcopy]\n";
    std::cout << "begin_string="    << cfg.get("dropcopy", "begin_string")    << "\n";
    std::cout << "server_ip="       << cfg.get("dropcopy", "server_ip")       << "\n";
    std::cout << "server_port="     << cfg.get("dropcopy", "server_port")     << "\n";
    std::cout << "sender_comp_id="  << cfg.get("dropcopy", "sender_comp_id")  << "\n";
    std::cout << "target_comp_id="  << cfg.get("dropcopy", "target_comp_id")  << "\n";
    std::cout << "username="        << cfg.get("dropcopy", "username")        << "\n";
    std::cout << "password="        << cfg.get("dropcopy", "password")        << "\n";
    std::cout << "reset_on_logon="  << cfg.get("dropcopy", "reset_on_logon")  << "\n";


    const std::string ip = cfg.get("dropcopy", "server_ip");
    const std::string port_text = cfg.get("dropcopy", "server_port");

    if (ip.empty() || port_text.empty()) {
        std::cerr << "missing server_ip/server_port in config\n";
        return 1;
    }

    const int port = std::stoi(port_text);

    Socket socket;

    if (!socket.connect_to_server(ip, static_cast<std::uint16_t>(port))) {
        std::cerr << "connect failed\n";
        return 1;
    }
    
    std::vector<char> buffer;
    int bytes_read = socket.recv_bytes(buffer, 4096);
    if (bytes_read <= 0) {
        std::cerr << "recv failed\n";
        return 1;
    }

    std::string inbound(buffer.data(), static_cast<std::size_t>(bytes_read));
    std::cout << "received: " << inbound << "\n";


    return 0;
}
