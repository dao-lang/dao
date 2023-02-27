#pragma once

#include <string>
#include <exception>

namespace dao
{
    class SyntaxError : public std::exception
    {
    public:
        SyntaxError();

        SyntaxError(const std::string &msg);

        virtual const char *what() const noexcept override;

    private:
        std::string msg;
    };
}