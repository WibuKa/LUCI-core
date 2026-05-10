#pragma once
#include "texture.h"
#include <cstdio>
#include <stdexcept>
#include "rect.h"

class TextureRegion {
private:
    Texture* texture = nullptr;

public:
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    
    TextureRegion() = default;

    TextureRegion(Texture* tex, int x, int y, int w, int h) {
        set_texture(tex);
        set_region(x, y, w, h);
    }

    Texture* get_texture(){
        return texture;
    }

    Rect get_rect() {
        return {x, y, w, h};
    }
    
    void set_texture(Texture* tex) {
        if (!tex) {
            throw std::runtime_error("TextureRegion: texture is null");
        }
        texture = tex;
    }

    void set_region(int x, int y, int w, int h) {
        printf("Region set: x=%d, y=%d, w=%d, h=%d\n", x, y, w, h);
        if (w <= 0 || h <= 0) {
            throw std::runtime_error("TextureRegion: invalid region size");
        }

        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
};

inline TextureRegion new_texture_region(Texture* tex, int x, int y, int w, int h) {
    printf("new_texture_region\n");
    return TextureRegion(tex, x, y, w, h);
}
