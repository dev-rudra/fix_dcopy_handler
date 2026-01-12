#include "seq_store.h"

#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

static const char* seq_dir_name = "tokens";

bool SeqStore::check_seq_dir() {
    struct stat st;
    if (::stat(seq_dir_name, &st) == 0) {
        return S_ISDIR(st.st_mode) != 0;
    }
    return ::mkdir(seq_dir_name, 0755) == 0;
}

std::string SeqStore::build_seq_path(const std::string& sender_comp_id,
                                     const std::string& target_comp_id) {
    return std::string(seq_dir_name) + "/" + sender_comp_id + "_" + target_comp_id + ".seq";
}

int SeqStore::load_next_outgoing_seq(const std::string& sender_comp_id,
                                     const std::string& target_comp_id) {
    if (sender_comp_id.empty() || target_comp_id.empty()) {
        return 1;
    }

    if (!check_seq_dir()) {
        return 1;
    }

    const std::string path = build_seq_path(sender_comp_id, target_comp_id);

    std::ifstream in(path.c_str());
    if (!in.is_open()) {
        return 1;
    }

    int next_outgoing_seq = 1;
    in >> next_outgoing_seq;

    if (!in.good() || next_outgoing_seq <= 0) {
        return 1;
    }

    return next_outgoing_seq;
}

bool SeqStore::save_next_outgoing_seq(const std::string& sender_comp_id,
                                      const std::string& target_comp_id,
                                      int next_outgoing_seq) {
    if (sender_comp_id.empty() || target_comp_id.empty()) {
        return false;
    }

    if (next_outgoing_seq <= 0) {
        return false;
    }

    if (!check_seq_dir()) {
        return false;
    }

    const std::string path = build_seq_path(sender_comp_id, target_comp_id);

    std::ofstream out(path.c_str(), std::ios::trunc);
    if (!out.is_open()) {
        return false;
    }

    out << next_outgoing_seq << "\n";
    return true;
}
