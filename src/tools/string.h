#ifndef STRING_H
#define STRING_H

#include <string>

class String {
public:
    static std::string to_cdn_name(const std::string &input) {
        std::string result;

        for (const char c: input) {
            if (std::isdigit(c)) {
                result += " ";
                result += c;
            } else {
                result += std::toupper(c);
            }
        }
        return result;
    }

    static std::string capitalize(std::string input) {
        if (!input.empty()) {
            input[0] = std::toupper(input[0]);
        }
        return input;
    }

    static std::string replace_substring(std::string str, const std::string &from, const std::string &to) {
        if (const size_t pos = str.find(from); pos != std::string::npos) {
            str.replace(pos, from.length(), to);
        }
        return str;
    }

    static void clean(std::string &str) {
        if (const size_t comment_pos = str.find("//"); comment_pos != std::string::npos) {
            str.erase(comment_pos);
        }

        str.erase(0, str.find_first_not_of(" \t"));
        str.erase(str.find_last_not_of(" \t") + 1);
        if (!str.empty() && str.back() == '\r')
            str.pop_back();
    }
};

#endif // STRING_H
