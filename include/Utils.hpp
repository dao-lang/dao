#pragma once

#include <string>

namespace dao
{
    std::string convert_string(const std::string& src);

    int get_indentation_count(const std::string& spaces);
}
