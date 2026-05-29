#pragma once
#include <string>
#include "sound.h"
#include "texture.h"
#include "texture_region.h"
#include "model.h"
#include "font.h"

namespace Loader {
    void init();

    Sound cloneSound(Sound& sound);
    Sound loadSound(std::string path);
    Sound loadStream(std::string path);

    Texture loadTexture(const std::string& path);
    TextureRegion newTextureRegion(Texture& tex, int x, int y, int w, int h);
    
    Font loadFont(const std::string& path,float font_size);

    Model* loadModel(const std::string &path);
}
