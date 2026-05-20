#pragma once
#include "glad/glad.h"
#include <cstdio>

class Texture {
private:
    unsigned int id = 0;
    int width = 0;
    int height = 0;
public:
    Texture() = default;
    Texture(unsigned int id, int w, int h)
        : id(id), width(w), height(h) {}
    unsigned int getID() const { return id; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};
