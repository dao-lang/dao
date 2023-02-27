#include "Utils.hpp"
#include "Error.hpp"

#include <sstream>
#include <string>

namespace dao {
    std::string convert_string(const std::string &src) {
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
                        throw SyntaxError("不支持的转义字符");
                }
                i += 2;
            } else {
                ss << c;
                i++;
            }
        }

        return ss.str();
    }

    int get_indentation_count(const std::string &spaces) {
        int count = 0;
        for (char ch : spaces) {
            switch (ch) {
                case '\t':
                    count += 8 - (count % 8);
                    break;
                default:
                    // A normal space char.
                    count++;
            }
        }

        return count;
    }
}