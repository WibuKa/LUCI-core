#pragma once
#include <memory>
#include <stdexcept>

class Texture {
private:
    unsigned int id = 0;
    int width = 0;
    int height = 0;

public:
    Texture() = default;
    Texture(unsigned int id, int w, int h)
        : id(id), width(w), height(h) {}

    int get_width() const { return width; }
    int get_height() const { return height; }
};

class TextureRegion {
private:
    std::shared_ptr<Texture> texture;

    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

public:
    TextureRegion() = default;

    TextureRegion(std::shared_ptr<Texture> tex,
                  int x, int y, int w, int h) {
        set_texture(tex);
        set_region(x, y, w, h);
    }

    TextureRegion(std::shared_ptr<Texture> tex) {
        set_texture(tex);

        if (tex) {
          x = 0;
          y = 0;
          w = tex->get_width();
          h = tex->get_height();
        }
    }

    void set_texture(std::shared_ptr<Texture> tex) {
        texture = tex;
    }

    void set_region(int x, int y, int w, int h) {
        if (!texture) {
            throw std::runtime_error("TextureRegion: texture is null");
        }

        if (w <= 0 || h <= 0) {
            throw std::runtime_error("Invalid region size");
        }

        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    void draw(float x, float y,  float angle = 0.0f, float scale_x = 1.0f, float scale_y = 1.0f) {}
};
