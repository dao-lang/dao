#pragma once

#include <string>
#include <exception>

namespace dao
{
    class SyntaxError : public std::exception
    {
    public:
        SyntaxError() : SyntaxError("语法错误") {}

        SyntaxError(const std::string &msg) : msg(msg) {}

        virtual const char *what() const noexcept override
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}