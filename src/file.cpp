#include "file.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace File
{
    std::string get_ext(const std::string& path)
    {
        std::filesystem::path filePath(path);
        return filePath.extension().string();
    }

    std::string read(const std::string& path)
    {
        std::ifstream file(path);
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return content;
    }
}
