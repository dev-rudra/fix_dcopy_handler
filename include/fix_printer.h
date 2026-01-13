#ifndef FIX_DCOPY_FIX_PRINTER_H
#define FIX_DCOPY_FIX_PRINTER_H

#include <string>
#include <vector>

struct fix_printer {
    std::string buf;

    // Parse ASCII digits in s[from..to] into int
    static int read_int(const std::string& s, size_t from, size_t to) {
        int v = 0;
        for (size_t i = from; i < to; ++i) {
            char c = s[i];
            if (c < '0' || c > '9') return -1;
            v = (v * 10) + (c - '0');
        }
        return v;
    }

    // Extract full FIX messages (8=.. 9=..) into out
    void feed(const char* data, size_t len, std::vector<std::string>& out) {
        buf.append(data, len);

        while (true) {
            size_t start = buf.find("8=FIX");
            if (start == std::string::npos) {
                // keep small tail so "8=FIX" can match across packet boundary
                if (buf.size() > 8) buf.erase(0, buf.size() - 8);
                return;
            }
            if (start > 0) buf.erase(0, start);

            // End of BeginString
            size_t begin_soh = buf.find('\x01');
            if (begin_soh == std::string::npos) return;
            
            // BodyLength is expected right after BeginString
            size_t bl_pos = begin_soh + 1;
            if (buf.size() < bl_pos + 3) return;

            if (buf[bl_pos] != '9' || buf[bl_pos + 1] != '=') {
                buf.erase(0, 1);
                continue;
            }

            size_t bl_soh = buf.find('\x01', bl_pos);
            if (bl_soh == std::string::npos) return;

            int body_len = read_int(buf, bl_pos + 2, bl_soh);
            if (body_len < 0) {
                buf.erase(0, 1);
                continue;
            }

            size_t header_len = bl_soh + 1;

            size_t total_len = header_len + (size_t)body_len + 7;

            if (buf.size() < total_len) return;
            out.push_back(buf.substr(0, total_len));
            buf.erase(0, total_len);
        }
    }

    // Convert SOH to '|'
    static std::string printable(const std::string& msg) {
        std::string s = msg;
        for (size_t i = 0; i < s.size(); ++i)
            if (s[i] == '\x01') s[i] = '|';
        return s;
    }
};

#endif
