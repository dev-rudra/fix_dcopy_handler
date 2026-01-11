#include "config_parser.h"

#include <cctype>
#include <fstream>
#include <sstream>

static std::string strip_whitespace(const std::string& s) {
    std::size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }

    std::size_t end = s.size();
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        end--;
    }

    return s.substr(start, end - start);
}

bool ConfigParser::read(const std::string& file_name) {
    cfg.clear();

    std::ifstream config_file(file_name.c_str());
    if (!config_file.is_open()) {
        return false;
    }

    std::string section;
    std::string line;

    while (std::getline(config_file, line)) {
        line = strip_whitespace(line);
        if (line.empty()) {
            continue;
        }

        if (line[0] == '#' || line[0] == ';') {
            continue;
        }

        if (line[0] == '[') {
            std::istringstream is_line(line);
            std::string ignore;
            std::string sec;

            std::getline(is_line, ignore, '[');
            std::getline(is_line, sec, ']');
            section = strip_whitespace(sec);
            continue;
        }

        std::istringstream is_line(line);
        std::string key;
        std::string value;

        if (std::getline(is_line, key, '=')) {
            if (std::getline(is_line, value)) {
                key = strip_whitespace(key);
                value = strip_whitespace(value);

                if (!section.empty() & !key.empty()) {
                    cfg[section][key] = value;
                }
            }
        }
    }
    return true;
}

std::string ConfigParser::get(const std::string& section, const std::string& key) const {
    try {
        const auto& sec = cfg.at(section);
        return sec.at(key);
    } catch (const std::out_of_range&) {
        return "";
    }
}
