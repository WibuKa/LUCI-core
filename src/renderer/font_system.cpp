#define STB_TRUETYPE_IMPLEMENTATION
#include "font_system.h"
#include "helper.h"
#include "render_system.h"
#include <fstream>
#include <vector>
#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <unordered_map>

namespace FontSYS {
std::string textToDisplay = "This is some text rendered in OpenGL.";

// Font Atlas settings:
const uint32_t codePointOfFirstChar = 32;      // ASCII of ' '(Space)
const uint32_t charsToIncludeInFontAtlas = 95; // Include 95 charecters

const uint32_t fontAtlasWidth  = 1024;
const uint32_t fontAtlasHeight = 1024;

struct Glyph
{
    float offset_x;
    float offset_y;
    float width;
    float height;
    float ascent;
    float advance;
};

struct Font
{
    stbtt_fontinfo info;
    float fontSize;
    float scale;
    float line_height;
    unsigned int textureID;
    std::unordered_map<uint32_t,Glyph> glyphs;
};

std::vector<Font> fonts;

int current_font = 0;

void set_font(int id)
{
    current_font = id;
}

unsigned int get_texture()
{
    return fonts[current_font].textureID;
}

unsigned int get_texture_id(int id)
{
    return fonts[id].textureID;
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
    return fonts[current_font].line_height;
}

float get_font_line_height(stbtt_fontinfo *font,float size) {
    float scale = stbtt_ScaleForPixelHeight(font, size);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(font, &ascent, &descent, &lineGap);
    return (ascent - descent + lineGap) * scale;
}

int load_font(const std::string& fontFile,float fontSize)
{
    stbtt_fontinfo fontInfo = {};
    
    std::ifstream inputStream(fontFile.c_str(), std::ios::binary);
    std::unordered_map<uint32_t,Glyph> glyph_map;

    inputStream.seekg(0, std::ios::end);
    auto&& fontFileSize = inputStream.tellg();
    inputStream.seekg(0, std::ios::beg);

    uint8_t* fontDataBuf = new uint8_t[fontFileSize];

    inputStream.read((char*)fontDataBuf, fontFileSize);

    uint32_t fontCount = stbtt_GetNumberOfFonts(fontDataBuf);

    if(!stbtt_InitFont(&fontInfo, fontDataBuf, 0))
        std::cerr << "stbtt_InitFont() Failed!\n";
    
    std::vector<int> codepoints = get_codepoints(&fontInfo);
    std::vector<stbtt_packedchar> packedChars(codepoints.size());

    uint8_t* fontAtlasTextureData = new uint8_t[fontAtlasWidth * fontAtlasHeight];
    
    stbtt_pack_context ctx;

    stbtt_PackBegin(&ctx, fontAtlasTextureData, fontAtlasWidth, fontAtlasHeight, 0, 1, nullptr);

    stbtt_pack_range range;
    range.font_size = fontSize;
    range.first_unicode_codepoint_in_range = 0;
    range.num_chars = codepoints.size();
    range.chardata_for_range = packedChars.data();
    range.array_of_unicode_codepoints = codepoints.data();

    stbtt_PackFontRanges(&ctx,fontDataBuf, 0, &range, 1);

    float scale = stbtt_ScaleForPixelHeight(&fontInfo, fontSize);

    for (int i = 0; i < codepoints.size(); i++)
    {
        float unusedX, unusedY;
        stbtt_aligned_quad quad;

        uint32_t codepoint = codepoints[i];
        uint32_t c = codepoint;

        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(
            &fontInfo,
            codepoints[i],
            scale, scale,
            &x0, &y0, &x1, &y1
        );

        float descent =  y1;
        float ascent  = -y0;

        float advance = packedChars[i].xadvance;

        glyph_map[c] = {
            (float)packedChars[i].x0,
            (float)packedChars[i].y0,
            (float)packedChars[i].x1 - packedChars[i].x0,
            (float)packedChars[i].y1 - packedChars[i].y0,
            ascent,
            advance
        };
    }

    stbtt_PackEnd(&ctx);

    GLuint textureID;
    textureID = RenderSYS::createTexture_data(fontAtlasTextureData,fontAtlasWidth,fontAtlasHeight,1,-1,-1);
    float line_height = fontSize;

    Font f;
    f.info = fontInfo;
    f.fontSize = fontSize;
    f.line_height = line_height;
    f.textureID = textureID;
    f.glyphs = glyph_map;
    fonts.push_back(std::move(f));
    delete[]fontAtlasTextureData;
    delete[]fontDataBuf;
    return fonts.size() - 1;
}

float get_kern(int char1,int char2) {
    return 0.0;
}

void get_glyph(uint32_t c, float &offset_x, float &offset_y, float &w, float &h, float &ascent, float &advance)
{
    Glyph glyph = fonts[current_font].glyphs[c];

    offset_x    = glyph.offset_x;
    offset_y    = glyph.offset_y;
    w           = glyph.width;
    h           = glyph.height;
    ascent      = glyph.ascent;
    advance     = glyph.advance;
}

}