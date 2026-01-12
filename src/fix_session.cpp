#include "fix_session.h"
#include "fix_message.h"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <string>

static std::string build_sending_time_utc() {
    using namespace std::chrono;

    const auto now = system_clock::now();
    const auto ms = static_cast<int>(duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000);

    const std::time_t t = system_clock::to_time_t(now);
    std::tm utc_tm {};
    gmtime_r(&t, &utc_tm);

    char ts[32];
    std::strftime(ts, sizeof(ts), "%Y%m%d-%H:%M:%S", &utc_tm);

    char out[40];
    std::snprintf(out, sizeof(out), "%s.%03d", ts, ms);

    return std::string(out);
}

// Minimal FIX session header fields
// (8,35,34,49,56,52)
static void add_session_header(FixMessage& msg,
                               const std::string& begin_string,
                               const std::string& msg_type,
                               int msg_seq_num,
                               const std::string& sender_comp_id,
                               const std::string& target_comp_id) {
    msg.add_string_field(8, begin_string);
    msg.add_string_field(35, msg_type);
    msg.add_int_field(34, msg_seq_num);
    msg.add_string_field(49, sender_comp_id);
    msg.add_string_field(56, target_comp_id);
    msg.add_string_field(52, build_sending_time_utc());
}

// Initialize session state
FixSession::FixSession()
    : heart_bt_int(30),
      reset_on_logon(false),
      outgoing_seq_num(1),
      expected_incoming_seq_num(1) {}

// Session configs from config.ini
void FixSession::set_session_values(const std::string& begin_string_value,
                                    const std::string& sender_comp_id_value,
                                    const std::string& target_comp_id_value,
                                    const std::string& username_value,
                                    const std::string& password_value,
                                    int heart_bt_int_value,
                                    bool reset_on_logon_value) {
    begin_string = begin_string_value;
    sender_comp_id = sender_comp_id_value;
    target_comp_id = target_comp_id_value;
    username = username_value;
    password = password_value;
    heart_bt_int = heart_bt_int_value;
    reset_on_logon = reset_on_logon_value;
}

// Set next outgoing MsgSeqNum
void FixSession::set_outgoing_seq_num(int next_outgoing_seq) {
    if (next_outgoing_seq > 0) {
        outgoing_seq_num = next_outgoing_seq;
    }
}

// Current outgoing SeqNumber (tag 34 for next send)
int FixSession::get_outgoing_seq_num() const {
    return outgoing_seq_num;
}

// Get expected incoming SeqNumber
int FixSession::get_expected_incoming_seq_num() const {
    return expected_incoming_seq_num;
}

// Validate recevd MsgSeqNum (tag 34)
bool FixSession::on_incoming_seq_num(int msg_seq_num) {
    if (msg_seq_num != expected_incoming_seq_num) {
        return false;
    }
    expected_incoming_seq_num += 1;
    return true;
}

// Logon 35=A
std::string FixSession::build_logon_message() {
    if (reset_on_logon) {
        outgoing_seq_num = 1;
        expected_incoming_seq_num = 1;
    }

    FixMessage msg;
    add_session_header(msg, begin_string, "A", outgoing_seq_num, sender_comp_id, target_comp_id);

    msg.add_int_field(98, 0);
    msg.add_int_field(108, heart_bt_int);
    msg.add_string_field(141, reset_on_logon ? "Y" : "N");

    if (!username.empty()) {
        msg.add_string_field(553, username);
    }
    if (!password.empty()) {
        msg.add_string_field(554, password);
    }

    outgoing_seq_num += 1;
    return msg.build_message();
}

// Heartbeat 35=0
std::string FixSession::build_heartbeat_message(const std::string& test_req_id) {
    FixMessage msg;
    add_session_header(msg, begin_string, "0", outgoing_seq_num, sender_comp_id, target_comp_id);

    if (!test_req_id.empty()) {
        msg.add_string_field(112, test_req_id);
    }

    outgoing_seq_num += 1;
    return msg.build_message();
}

// Logout 35=5
std::string FixSession::build_logout_message(const std::string& logout_reason) {
    FixMessage msg;
    add_session_header(msg, begin_string, "5", outgoing_seq_num, sender_comp_id, target_comp_id);

    if (!logout_reason.empty()) {
        msg.add_string_field(58, logout_reason);
    }

    outgoing_seq_num += 1;
    return msg.build_message();
}

// ResendRequest 35=2 with 7=BeginSeqNo and 16=EndSeqNo (0 = infinity).
std::string FixSession::build_resend_request_message(int begin_seq_no, int end_seq_no) {
    FixMessage msg;
    add_session_header(msg, begin_string, "2", outgoing_seq_num, sender_comp_id, target_comp_id);

    msg.add_int_field(7, begin_seq_no);
    msg.add_int_field(16, end_seq_no);

    outgoing_seq_num += 1;
    return msg.build_message();
}
