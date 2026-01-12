#include <iostream>
#include <string>

#include "fix_session.h"

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
    FixSession session;

    session.set_session_values(
        "FIX.4.4",      // begin_string
        "DROP1",        // sender_comp_id
        "Exchange",     // target_comp_id
        "username",     // username
        "password",     // password
        30,             // heart_bt_int
        true            // reset_on_logon
    );

    session.set_token(""); // keep empty for now

    const std::string logon = session.build_logon_message();
    std::cout << "LOGON:   " << show_fix(logon) << "\n";

    const std::string hb = session.build_heartbeat_message("");
    std::cout << "HB:      " << show_fix(hb) << "\n";

    const std::string logout = session.build_logout_message("qa test logout");
    std::cout << "LOGOUT:  " << show_fix(logout) << "\n";

    const std::string rr = session.build_resend_request_message(1, 0);
    std::cout << "RESEND:  " << show_fix(rr) << "\n";

    std::cout << "out_seq=" << session.get_outgoing_seq_num()
              << " expected_in_seq=" << session.get_expected_incoming_seq_num()
              << "\n";

    return 0;
}
