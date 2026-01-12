#include <iostream>
#include <string>

#include "fix_message.h"

static std::string show_fix(const std::string& msg) {
    std::string out = msg;
    for (char& c : out) {
        if (c == '\x01') {
            c = '|';
        }
    }
    return out;
}

int main() {
    FixMessage msg;

    msg.add_string_field(8, "FIX.4.4");     // BeginString
    msg.add_string_field(35, "A");          // Logon
    msg.add_int_field(34, 1);               // MsgSeqNum
    msg.add_string_field(49, "DROP1");      // SenderCompID
    msg.add_string_field(56, "Exchange");   // TargetCompID
    msg.add_int_field(98, 0);               // EncryptMethod
    msg.add_int_field(108, 30);             // HeartBtInt
    msg.add_string_field(141, "Y");         // ResetSeqNumFlag
    msg.add_string_field(553, "username");  // Username
    msg.add_string_field(554, "password");  // Password

    const std::string wire_message = msg.build_message();
    if (wire_message.empty()) {
        std::cerr << "failed to build FIX message (missing 8=BeginString?)\n";
        return 1;
    }

    std::cout << show_fix(wire_message) << "\n";
    return 0;
}
