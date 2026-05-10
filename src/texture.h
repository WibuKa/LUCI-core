#pragma once
#include "glad/glad.h"
#include <cstdio>
#include <string>

class Texture {
private:
    unsigned int id = 0;
    int width = 0;
    int height = 0;

public:
    Texture() = default;
    Texture(unsigned int id, int w, int h)
        : id(id), width(w), height(h) {}
    unsigned int get_id() const { return id; }
    int get_width() const { return width; }
    int get_height() const { return height; }
};


Texture create_texture(const std::string &path);
Texture* load_texture(const std::string& path);
