#ifndef FIX_DCOPY_SEQ_STORE_H
#define FIX_DCOPY_SEQ_STORE_H

#include <string>

class SeqStore {
public:
    // Read SeqNum from .seq file if not in file returns 1
    int load_next_outgoing_seq(const std::string& sender_comp_id,
                               const std::string& target_comp_id);

    // Save next outgoing SeqNum 
    bool save_next_outgoing_seq(const std::string& sender_comp_id,
                                const std::string& target_comp_id,
                                int next_outgoing_seq);

private:
    bool check_seq_dir();
    std::string build_seq_path(const std::string& sender_comp_id,
                               const std::string& target_comp_id);
};

#endif

