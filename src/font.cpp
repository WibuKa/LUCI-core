#define STB_TRUETYPE_IMPLEMENTATION
#include "font.h"
#include <vector>
#include <cstdint>


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
    for (const auto g : *glyphs)
    {
        if (g.codepoint == c)
            return g;
    }
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

}
