#pragma once
#include <string>
#include <vector>
#include "texture.h"

class Material {
public:
    std::string name;
    std::vector<Texture> textures;
};
