#define STB_TRUETYPE_IMPLEMENTATION
#include "font.h"
#include <vector>
#include <cstdint>
#include <ft2build.h>
#include FT_FREETYPE_H

const uint32_t codePointOfFirstChar = 32;
const uint32_t charsToIncludeInFontAtlas = 95;

// -------------------------- Font Class --------------------------- //
bool Font::isValid(){
    return valid;
}
std::string Font::getPath(){
    return path;
}
unsigned int Font::getTextureID(){
    return texture.getID();
}
Texture Font::getTexture(){
    return texture;
}
Glyph Font::getGlyph(uint32_t c)
{
    auto it = glyphs->find(c);
    if (it != glyphs->end())
        return it->second;
    return Glyph{};
}
float Font::getScale(){
    return scale;
}
float Font::getLineHeight(){
    return line_height;
}
float Font::getLetterSpacing(){
    return letter_spacing;
}
float Font::getAscent(){
    return ascent;
}
void Font::setScale(float scale){
    this->scale = scale;
}
void Font::setLineHeight(float line_height){
    this->line_height = line_height;
}
void Font::setLetterSpacing(float letter_spacing){
    this->letter_spacing = letter_spacing;
}
// ------------------------------------------------------------------- //

namespace FontSYS {
std::string textToDisplay = "This is some text rendered in OpenGL.";

int current_font = 0;

void set_font(int id)
{
    current_font = id;
}

unsigned int get_texture()
{
    //return fonts[current_font].textureID;
}

unsigned int get_texture_id(int id)
{
    //return fonts[id].textureID;
}

std::vector<int> get_codepoints(stbtt_fontinfo* font)
{
    std::vector<int> cps;

    for (int cp = 0x000000; cp <= 0x10FFFF; cp++)
    {
        int glyph = stbtt_FindGlyphIndex(font, cp);
        if (glyph != 0)
            cps.push_back(cp);
    }
    return cps;
}

float get_line_height()
{
    //return fonts[current_font].line_height;
}

float get_font_line_height(stbtt_fontinfo *font,float size) {
    float scale_for_pixel = stbtt_ScaleForPixelHeight(font, size);
    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(font, &ascent, &descent, &line_gap);
    return (ascent - descent + line_gap) * scale_for_pixel;
}

float get_kern(int char1,int char2) {
    return 0.0;
}

Glyph get_glyph(Font font, uint32_t c)
{
    return font.getGlyph(c);
}
}
