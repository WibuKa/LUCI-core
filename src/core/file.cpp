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
}