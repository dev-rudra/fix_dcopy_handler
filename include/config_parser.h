#ifndef FIX_DCOPY_CONFIG_PARSER_H
#define FIX_DCOPY_CONFIG_PARSER_H

#include <map>
#include <string>

class ConfigParser {
public:
    bool read(const std::string& path);
    std::string get(const std::string& section, const std::string& key) const;

    using KeyValueMap = std::map<std::string, std::string>;
    using SectionMap = std::map<std::string, KeyValueMap>;

    SectionMap cfg;
};

#endif
