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
    std::string name;
    Texture() = default;
    Texture(unsigned int id, int w, int h)
        : id(id), width(w), height(h) {}
    void create(const unsigned char* data, int w, int h, int channels);
    
    unsigned int getID() const { return id; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};
