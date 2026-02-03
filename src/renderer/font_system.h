#pragma once
#include "stb/stb_truetype.h"
#include <string>
#include <cstdint>

namespace FontSYS {
    int load_font(const std::string& fontFile,float fontSize);
    void set_font(int id);
    float get_line_height();
    unsigned int get_texture();
    unsigned int get_texture_id(int id);
    float get_kern(int char1,int char2);
    void get_glyph(uint32_t c, float &offset_x, float &offset_y, float &w, float &h, float &descent,float &advance);
}
