#pragma once
#include <string>


namespace File
{
    std::string get_ext(const std::string& path);
    std::string read(const std::string& path);
}
