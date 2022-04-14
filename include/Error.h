#pragma once

#include <string>
#include <exception>

class SyntaxError : public std::exception
{
public:
    SyntaxError();
    
    SyntaxError(const std::string &msg);

    virtual const char *what() const noexcept override;

private:
    std::string msg;
};
