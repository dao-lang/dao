#include "Error.h"

namespace dao
{
    SyntaxError::SyntaxError() : SyntaxError("语法错误")
    {
    }

    SyntaxError::SyntaxError(const std::string &msg) : msg(msg)
    {
    }

    const char *SyntaxError::what() const noexcept
    {
        return msg.c_str();
    }
}