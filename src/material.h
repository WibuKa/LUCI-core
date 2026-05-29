#pragma once
#include <string>
#include "texture.h"
#include <vector>

#define BASE_COLOR_TEXTURE_SLOT 0
#define NORMAL_TEXTURE_SLOT 1
#define METALLIC_TEXTURE_SLOT 2
#define OCCLUSION_TEXTURE_SLOT 3
#define EMISSIVE_TEXTURE_SLOT 4

class Material {
public:
    std::string name;
    unsigned int shader;
    
    std::vector<Texture> textures;

    Material() {
        textures.resize(5); 
    }
};
