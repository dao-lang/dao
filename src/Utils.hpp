#pragma once

#include <string>
#include <sstream>

namespace dao {
    std::string convert_string(const std::string src) {
        std::stringstream ss;

        for (size_t i = 1; i < src.length() - 1;) {
            auto c = src[i];
            if (c == '\\') {
                switch (src[i + 1]) {
                    case 'n':
                        ss << '\n';
                        break;

                    case 't':
                        ss << '\t';
                        break;

                    case 'r':
                        ss << '\r';
                        break;

                    case '0':
                        ss << '\0';
                        break;

                    default:
                        throw "不支持的转义字符";
                }
                i += 2;
            } else {
                ss << c;
                i++;
            }
        }

        return ss.str();
    }
}
