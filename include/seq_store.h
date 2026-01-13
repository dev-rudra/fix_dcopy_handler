#ifndef FIX_DCOPY_SEQ_STORE_H
#define FIX_DCOPY_SEQ_STORE_H

#include <string>

class SeqStore {
public:
    // Read outgoing SeqNum from *.out.seq (default 1)
    int load_next_outgoing_seq(const std::string& sender_comp_id,
                               const std::string& target_comp_id);

    // Save outgoing SeqNum to *.out.seq
    bool save_next_outgoing_seq(const std::string& sender_comp_id,
                                const std::string& target_comp_id,
                                int next_outgoing_seq);

    // Read expected incoming SeqNum from *.in.seq (default 1)
    int load_expected_incoming_seq(const std::string& sender_comp_id,
                                   const std::string& target_comp_id);

    // Save expected incoming SeqNum to *.in.seq
    bool save_expected_incoming_seq(const std::string& sender_comp_id,
                                    const std::string& target_comp_id,
                                    int expected_incoming_seq);

private:
    bool check_seq_dir();

    std::string build_out_seq_path(const std::string& sender_comp_id,
                                   const std::string& target_comp_id);

    std::string build_in_seq_path(const std::string& sender_comp_id,
                                  const std::string& target_comp_id);
};

#endif

