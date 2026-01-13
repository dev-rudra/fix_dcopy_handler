#include "application.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "config_parser.h"
#include "fix_printer.h"
#include "fix_session.h"
#include "seq_store.h"
#include "socket.h"

static std::string soh_to_pipe(const std::string& fix_data) {
    std::string printable = fix_data;
    for (char& ch : printable) {
        if (ch == '\x01') {
            ch = '|';
        }
    }
    return printable;
}

static std::string get_tag(const std::string& fix, const char* tag) {
    const char soh = '\x01';
    const std::string key = std::string(tag) + "=";

    size_t pos = 0;
    while (true) {
        size_t end = fix.find(soh, pos);
        if (end == std::string::npos) break;

        if (fix.compare(pos, key.size(), key) == 0) {
            return fix.substr(pos + key.size(), end - (pos + key.size()));
        }

        pos = end + 1;
    }

    return "";
}

int Application::run() {
    ConfigParser cfg;
    if (!cfg.read("cfg/config.ini")) {
        std::cerr << "failed to open cfg/config.ini\n";
        return 1;
    }

    const std::string begin_string = cfg.get("dropcopy", "begin_string");
    const std::string server_ip = cfg.get("dropcopy", "server_ip");
    const std::string server_port_str = cfg.get("dropcopy", "server_port");
    const std::string sender_comp_id = cfg.get("dropcopy", "sender_comp_id");
    const std::string target_comp_id = cfg.get("dropcopy", "target_comp_id");
    const std::string username = cfg.get("dropcopy", "username");
    const std::string password = cfg.get("dropcopy", "password");
    const std::string reset_on_logon_str = cfg.get("dropcopy", "reset_on_logon");
    const std::string heartbeat_interval_str = cfg.get("dropcopy", "heartbeat_interval");

    const int server_port = std::stoi(server_port_str);
    const bool reset_on_logon = (reset_on_logon_str == "true");

    int heart_bt_int = 30;
    if (!heartbeat_interval_str.empty()) {
        heart_bt_int = std::stoi(heartbeat_interval_str);
    }

    FixSession session;
    session.set_session_values(
        begin_string,
        sender_comp_id,
        target_comp_id,
        username,
        password,
        heart_bt_int,
        reset_on_logon
    );

    SeqStore seq_store;

    if (!reset_on_logon) {
        const int next_out_seq = seq_store.load_next_outgoing_seq(sender_comp_id, target_comp_id);
        session.set_outgoing_seq_num(next_out_seq);
    }

    Socket socket;
    if (!socket.connect_to_server(server_ip, static_cast<std::uint16_t>(server_port))) {
        std::cerr << "connect failed: " << server_ip << ":" << server_port << "\n";
        return 1;
    }

    std::cout << "connected: " << server_ip << ":" << server_port << "\n";

    const std::string logon_message = session.build_logon_message();
    if (!socket.send_bytes(logon_message)) {
        std::cerr << "failed to send logon\n";
        return 1;
    }

    seq_store.save_next_outgoing_seq(sender_comp_id, target_comp_id, session.get_outgoing_seq_num());

    std::cout << "sent logon: " << soh_to_pipe(logon_message) << "\n";

    fix_printer printer;

    while (true) {
        std::vector<char> buffer;
        const int bytes_read = socket.recv_bytes(buffer, 4096);
        if (bytes_read <= 0) {
            std::cout << "disconnected\n";
            break;
        }

        std::vector<std::string> msgs;
        printer.feed(buffer.data(), (size_t)bytes_read, msgs);

        for (size_t i = 0; i < msgs.size(); ++i) {
            const std::string& raw = msgs[i];

            const std::string msg_type = get_tag(raw, "35");
            const std::string seq_num = get_tag(raw, "34");

            std::cout << "[recv] < " << fix_printer::printable(raw) << "\n";

            if (msg_type == "1") {
                const std::string test_req_id = get_tag(raw, "112");

                const std::string heart_beat = session.build_heartbeat_message(test_req_id);
                if (socket.send_bytes(heart_beat)) {
                    seq_store.save_next_outgoing_seq(sender_comp_id, target_comp_id, session.get_outgoing_seq_num());
                    std::cout << "Sent HB: " << fix_printer::printable(heart_beat) << "\n";
                }
            }

            // Logout -> exit
            if (msg_type == "5") {
                std::cout << "logout received\n";
                return 0;
            }
        }
    }

    return 0;
}
