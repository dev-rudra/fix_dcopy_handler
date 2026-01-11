#include "token_handler.h"

#include <ctime>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

static const char* tokens_dir_name = "tokens";

bool TokenHandler::check_tokens_dir() {
    struct stat st;
    if (::stat(tokens_dir_name, &st) == 0) {
        return S_ISDIR(st.st_mode) != 0;
    }

    return ::mkdir(tokens_dir_name, 0755) == 0;
}

std::string TokenHandler::get_today_date() {
    const std::time_t now_time = std::time(nullptr);

    std::tm local_tm {};
    localtime_r(&now_time, &local_tm);

    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y%m%d", &local_tm);

    return std::string(buf);
}

std::string TokenHandler::build_token_path(const std::string& sender_comp_id) {
    const std::string today_date = get_today_date();
    return std::string(tokens_dir_name) + "/" + sender_comp_id + "_" + today_date + ".token";
}

bool TokenHandler::create_token(const std::string& sender_comp_id) {
    if (sender_comp_id.empty()) {
        return false;
    }

    if (!check_tokens_dir()) {
        return false;
    }

    const std::string path = build_token_path(sender_comp_id);
    std::ofstream out(path.c_str(), std::ios::app);
    return out.is_open();
}

std::string TokenHandler::get_token(const std::string& sender_comp_id) {
    if (sender_comp_id.empty()) {
        return "";
    }

    if (!check_tokens_dir()) {
        return "";
    }

    const std::string path = build_token_path(sender_comp_id);

    std::ifstream in(path.c_str());
    if (!in.is_open()) {
        create_token(sender_comp_id);
        return "";
    }

    std::string line;
    std::getline(in, line);
    return line;
}
