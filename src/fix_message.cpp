#include "fix_message.h"

#include <cstdio>
#include <string>

static const char soh = '\x01';

void FixMessage::clear() {
    fields.clear();
}

void FixMessage::add_string_field(int tag, const std::string& value) {
    fields.push_back(std::make_pair(tag, value));
}

void FixMessage::add_int_field(int tag, int value) {
    fields.push_back(std::make_pair(tag, std::to_string(value)));
}

std::string FixMessage::get_body(const std::vector<std::pair<int, std::string>>& fields) {
    std::string body;

    for (const auto&kv : fields) {
        const int tag = kv.first;

        if (tag == 8 || tag == 9 || tag == 10) {
            continue;
        }

        body += std::to_string(tag);
        body += '=';
        body += kv.second;
        body += soh;
    }

    return body;
}

int FixMessage::get_body_length(const std::string& body) {
    return static_cast<int>(body.size());
}

unsigned int FixMessage::get_checksum(const std::string& message_data) {
    unsigned int sum = 0;
    for (unsigned char c : message_data) {
        sum += static_cast<unsigned int>(c);
    }
    return sum % 256;
}

// Formats checksum as FIX
// requires 3 digits, zero-padded value
// e.g., 5 -> "005", 45 -> "045", and 255 -> "255"
std::string FixMessage::zero_pad_checksum(unsigned int checksum_value) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%03u", checksum_value);
    return std::string(buf);
}

std::string FixMessage::build_message() const {
    std::string begin_string;

    for (const auto& kv : fields) {
        if (kv.first ==8) {
            begin_string = kv.second;
            break;
        }
    }

    if (begin_string.empty()) {
        return "";
    }

    const std::string body = get_body(fields);
    const int body_length = get_body_length(body);

    std::string message_without_checksum;
    message_without_checksum += "8=";
    message_without_checksum += begin_string;
    message_without_checksum += soh;

    message_without_checksum += "9=";
    message_without_checksum += std::to_string(body_length);
    message_without_checksum += soh;

    message_without_checksum += body;

    // Checksum value
    const unsigned int checksum_value = get_checksum(message_without_checksum);
    const std::string checksum_str = zero_pad_checksum(checksum_value);

    std::string wire_message = message_without_checksum;
    wire_message += "10=";
    wire_message += checksum_str;
    wire_message += soh;

    return wire_message;
}
