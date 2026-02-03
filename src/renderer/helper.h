#pragma once
#include <vector>
#include <string>
#include <cstdint>

bool saveTextureToPNG(const char* filename, unsigned int textureID, int width, int height, int channels);
std::vector<uint32_t> string2U32(const std::string& s);