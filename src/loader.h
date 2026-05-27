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
    Sound load_sound(std::string path);
    Sound load_stream(std::string path);

    Texture load_texture(const std::string& path);
    TextureRegion new_texture_region(Texture& tex, int x, int y, int w, int h);
    
    Font loadFont(const std::string& path,float font_size);

    Model loadModel(const std::string &path);
}
