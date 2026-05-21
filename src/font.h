#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <vector>
#include "texture.h"

#define LEFT_AlIGN   0
#define CENTER_ALIGN 1
#define RIGHT_ALIGN  2

struct Glyph
{
    uint32_t codepoint;
    float    advance;
    float    offset_x;
    float    offset_y;
    float    width;
    float    height;

    float u0, v0;
    float u1, v1;
    float u2, v2;
    float u3, v3;
};

class Font
{
private:
    bool valid           = false;
    std::string path     = "";
    Texture texture      = {};

    float scale          = 1.0f;
    float ascent         = 0.0f;
    float descent        = 0.0f;
    float line_gap       = 0.0f;
    float line_height    = 0.0f;
    float letter_spacing = 0.0f;

    std::shared_ptr<std::vector<Glyph>> glyphs;

public:

    Font() = default;

    Font(
        bool valid,
        const std::string& path,
        const Texture& tex,
        float scale,
        float ascent,
        float descent,
        float line_gap,
        float line_height,
        float letter_spacing,
        std::shared_ptr<std::vector<Glyph>> glyphs
    )
        : valid(valid),
          path(path),
          texture(tex),
          scale(scale),
          ascent(ascent),
          descent(descent),
          line_gap(line_gap),
          line_height(line_height),
          letter_spacing(letter_spacing),
          glyphs(glyphs)
    {
    }

    bool isValid();

    std::string getPath();
    unsigned int getTextureID();
    Texture getTexture();

    Glyph getGlyph(uint32_t c);
    float getScale();
    float getLineHeight();
    float getLetterSpacing();
    float getAscent();
    
    void setScale(float scale);
    void setLineHeight(float line_height);
    void setLetterSpacing(float letter_spacing);
};

