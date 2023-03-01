#include "Error.hpp"

#include <utility>

namespace dao
{
    SyntaxError::SyntaxError() : SyntaxError("语法错误")
    {
    }

    SyntaxError::SyntaxError(std::string msg) : msg(std::move(msg))
    {
    }

    const char *SyntaxError::what() const noexcept
    {
        return msg.c_str();
    }
}