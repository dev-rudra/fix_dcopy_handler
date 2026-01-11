#ifndef FIX_DCOPY_TOKEN_HANDLER_H
#define FIX_DCOPY_TOKEN_HANDLER_H

#include <string>

class TokenHandler {
public:
    std::string get_token(const std::string& sender_comp_id);
    bool create_token(const std::string& sender_comp_id);

private:
    bool check_tokens_dir();
    std::string get_today_date();
    std::string build_token_path(const std::string& sender_comp_id);
};

#endif
