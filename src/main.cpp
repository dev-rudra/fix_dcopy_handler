#include <iostream>
#include "token_handler.h"

int main() {
    TokenHandler th;

    const std::string sender = "DROP1";

    const bool created = th.create_token(sender);
    const std::string token = th.get_token(sender);

    std::cout << "token_file_created=" << (created ? "true" : "false") << "\n";
    std::cout << "token_length=" << token.size() << "\n";

    return 0;
}
