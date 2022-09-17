#pragma once

#include <string>
#include <exception>
#include <utility>

namespace dao {
    class SyntaxError : public std::exception {
    public:
        SyntaxError() : SyntaxError("语法错误") {}

        explicit SyntaxError(std::string msg) : msg(std::move(msg)) {}

        const char *what() const noexcept override { return msg.c_str(); }

    private:
        std::string msg;
    };
}