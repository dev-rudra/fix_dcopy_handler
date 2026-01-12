#ifndef FIX_DCOPY_FIX_MESSAGE_H
#define FIX_DCOPY_FIX_MESSAGE_H

#include <string>
#include <utility>
#include <vector>

class FixMessage {
public:
    void clear();

    void add_string_field(int tag, const std::string& value);
    void add_int_field(int tag, int value);

    // Serialize message
    std::string build_message() const;

private:
    static std::string get_body(const std::vector<std::pair<int, std::string>>& fields);
    static int get_body_length(const std::string& body);
    static unsigned int get_checksum(const std::string& message_data);
    static std::string zero_pad_checksum(unsigned int checksum_value);

    std::vector<std::pair<int, std::string>> fields;
};

#endif
