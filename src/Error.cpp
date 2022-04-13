#include "Error.h"

SyntaxError::SyntaxError() : SyntaxError("语法错误")
{
}

SyntaxError::SyntaxError(const std::string &msg) : msg(msg)
{
}

const char *SyntaxError::what() const _NOEXCEPT
{
    return msg.c_str();
}