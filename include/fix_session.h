#ifndef FIX_DCOPY_FIX_SESSION_H
#define FIX_DCOPY_FIX_SESSION_H

#include <string>

class FixSession {
public:
    FixSession();

    void set_session_values(const std::string& begin_string,
                            const std::string& sender_comp_id,
                            const std::string& target_comp_id,
                            const std::string& username,
                            const std::string& password,
                            int heart_bt_int,
                            bool reset_on_logon);

    void set_token(const std::string& token_value);

    std::string build_logon_message();
    std::string build_heartbeat_message(const std::string& test_req_id);
    std::string build_logout_message(const std::string& logout_reason);
    std::string build_resend_request_message(int begin_seq_no, int end_seq_no);

    int get_outgoing_seq_num() const;
    int get_expected_incoming_seq_num() const;

    // Update expected incoming SeqNumber from received 34=...
    // Returns true if msg_seq_num matches expected;
    // false if gap/duplicate detected.
    bool on_incoming_seq_num(int msg_seq_num);

private:
    std::string begin_string;
    std::string sender_comp_id;
    std::string target_comp_id;
    std::string username;
    std::string password;
    std::string token_value;

    int heart_bt_int;
    bool reset_on_logon;

    int outgoing_seq_num;
    int expected_incoming_seq_num;
};

#endif
