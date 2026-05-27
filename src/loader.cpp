#include <stb_image_write.h>
#include <stb_image.h>
#include <unordered_map>
#include <memory>
#include "loader.h"
#include "delog.h"
#include "audio.h"
#include "system.h"
#include "texture.h"
#include "renderer.h"
#include <ft2build.h>
#include FT_FREETYPE_H

std::unordered_map<std::string, size_t> textures_cache;
std::vector<Texture> loaded_textures;

namespace Loader {

    void init()
    {
        std::vector<std::string> fontNames;

        if (System::operatingSystem == WINDOWS_OS)
        {
            fontNames = {
                "C:/Windows/Fonts/consolas.ttf",
                "C:/Windows/Fonts/cour.ttf",
                "C:/Windows/Fonts/lucon.ttf",
                "C:/Windows/Fonts/msgothic.ttc"
            };
        }
        else if (System::operatingSystem == LINUX_OS)
        {
            fontNames = {
                "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
               "/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf",
                "/usr/share/fonts/truetype/freefont/FreeMono.ttf"
            };
        }

        Font defaultFont;

        for (const auto& name : fontNames)
        {
            defaultFont = loadFont(name.c_str(), 16);
            if (defaultFont.isValid())
            {
                break; 
            }
        }

        Render::setFont(defaultFont);
    }

    Font loadFont(const std::string& path, float font_size)
    {
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            Delog::error("ERROR::FREETYPE: Could not init FreeType Library");
            return {};
        }

        FT_Face face;
        if (FT_New_Face(ft, path.c_str(), 0, &face))
        {
            Delog::error("ERROR::FREETYPE: Could not load font");
            FT_Done_FreeType(ft);
            return {};
        }

        FT_Set_Pixel_Sizes(face, 0, (FT_UInt)font_size);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        struct GlyphBitmap 
        {
            uint32_t codepoint;
            float    advance;
            int      width;
            int      height;
            float    offset_x; 
            float    offset_y;
            int      atlas_x;
            int      atlas_y;
        };

        std::vector<GlyphBitmap> temp_glyphs;
        temp_glyphs.reserve(96);

        for (unsigned char c = 32; c < 128; c++)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT))
                continue;

            GlyphBitmap glyph_bitmap;
            glyph_bitmap.codepoint = c;
            glyph_bitmap.width     = (int)face->glyph->bitmap.width;
            glyph_bitmap.height    = (int)face->glyph->bitmap.rows;
            glyph_bitmap.offset_x  = (float)face->glyph->bitmap_left;
            glyph_bitmap.offset_y  = (float)face->glyph->bitmap_top;
            glyph_bitmap.advance   = (float)face->glyph->advance.x / 64.0f; 
            glyph_bitmap.atlas_x   = 0;
            glyph_bitmap.atlas_y   = 0;

            temp_glyphs.push_back(glyph_bitmap);
        }

        int atlasSize = 512;
        bool packing_success = false;
        
        while (!packing_success)
        {
            int penX        = 0;
            int penY        = 0;
            int rowHeight   = 0;
            packing_success = true;

            for (GlyphBitmap& g : temp_glyphs)
            {
                if (penX + g.width >= atlasSize) {
                    penX = 0;
                    penY += rowHeight;
                    rowHeight = 0;
                }

                if (penY + g.height >= atlasSize) {
                    packing_success = false;
                    atlasSize *= 2;
                    break;
                }

                g.atlas_x = penX;
                g.atlas_y = penY;

                penX += g.width;
                rowHeight = std::max(rowHeight, g.height);
            }
        }
        size_t dataSize = (size_t)atlasSize * atlasSize;
        std::vector<uint8_t> atlasData(dataSize, 0);

        for (const auto& g : temp_glyphs)
        {
            if (g.width == 0 || g.height == 0) continue;

            FT_Load_Char(face, g.codepoint, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT);
            FT_Bitmap& bitmap = face->glyph->bitmap;

            for (int row = 0; row < g.height; ++row)
            {
                for (int col = 0; col < g.width; ++col)
                {
                    uint8_t pixelValue = bitmap.buffer[row * bitmap.pitch + col];
                    size_t destIndex = (size_t)(g.atlas_y + row) * atlasSize + (g.atlas_x + col);
                    atlasData[destIndex] = pixelValue;
                }
            }
        }

        float ascent      = (float)face->size->metrics.ascender  / 64.0f;
        float descent     = (float)face->size->metrics.descender / 64.0f;
        float line_height = (float)face->size->metrics.height    / 64.0f;
        float line_gap    = line_height - ascent + descent;
        
        std::vector<Glyph> glyphs;
        float fAtlasSize = (float)atlasSize;
        for (const auto& g : temp_glyphs)
        {
            Glyph glyph;
            glyph.codepoint = g.codepoint;
            glyph.advance   = g.advance;
            glyph.offset_x  = g.offset_x; 
            glyph.offset_y  = g.offset_y;
            glyph.width     = g.width;
            glyph.height    = g.height;

            float x_start = (float)g.atlas_x;
            float y_start = (float)g.atlas_y;
            float x_end   = (float)(g.atlas_x + g.width);
            float y_end   = (float)(g.atlas_y + g.height);

            glyph.u0        = x_start / fAtlasSize;
            glyph.v0        = y_start / fAtlasSize;
            glyph.u1        = x_end / fAtlasSize;
            glyph.v1        = y_start / fAtlasSize;
            glyph.u2        = x_end / fAtlasSize;
            glyph.v2        = y_end / fAtlasSize;
            glyph.u3        = x_start / fAtlasSize;
            glyph.v3        = y_end / fAtlasSize;

            glyphs.push_back(glyph);
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        Texture texture = Render::createTexture(atlasData.data(), atlasSize, atlasSize, 1);

        return Font(
            true, 
            path,
            texture, 
            1, 
            ascent, 
            descent,
            line_gap, 
            line_height, 
            0,
            std::make_shared<std::vector<Glyph>>(glyphs)
        );
    }

    Sound loadSound(std::string path)
    {
        auto ptr = std::make_shared<ma_sound>();
        ma_result result = ma_sound_init_from_file(
            &Audio::engine,
            path.c_str(),
            0,
            NULL,
            NULL,
            ptr.get()
        );

        if (result != MA_SUCCESS) {
            Delog::msg("Failed to load sound: %s", path.c_str());
            return {};
        }
        return Sound(ptr, path, false);
    }

    Sound cloneSound(Sound& sound)
    {
        if(sound.isStream()){
            auto clone = std::make_shared<ma_sound>();
            std::string path = sound.getPath();
            if(ma_sound_init_from_file(
                &Audio::engine,
                path.c_str(),
                MA_SOUND_FLAG_STREAM,
                NULL,
                NULL,
                clone.get()
            ) != MA_SUCCESS){
                Delog::msg("Failed to clone audio");
                return {};
            }
            return Sound(clone,path,true);
        }
        else {
            auto clone = std::make_shared<ma_sound>();
            if(ma_sound_init_copy(&Audio::engine, 
                sound.get(), 
                0,
                nullptr,
                clone.get()
            ) != MA_SUCCESS){
                Delog::msg("Failed to clone audio");
                return {};
            }
            return Sound(clone,sound.getPath(),false);
        }
    }

    Sound loadStream(std::string path)
    {
        auto ptr = std::make_shared<ma_sound>();
        ma_result result = ma_sound_init_from_file(
            &Audio::engine,
            path.c_str(),
            MA_SOUND_FLAG_STREAM,
            NULL,
            NULL,
            ptr.get()
        );

        if (result != MA_SUCCESS) {
            Delog::msg("Failed to load music: %s", path.c_str());
            return {};
        }

        return Sound(ptr, path, true);
 
    }


    Texture loadTexture(const std::string& path){
        auto it = textures_cache.find(path);
        if (it != textures_cache.end()) {
            return loaded_textures[it->second];
        }

        size_t index = loaded_textures.size();

        int width, height, channel;
        uint8_t* data = stbi_load(path.c_str(), &width, &height, &channel, 0);
        Texture tex  = Render::createTexture(data, width, height, channel);
        stbi_image_free(data);
        loaded_textures.push_back(tex);
        textures_cache[path] = index;
        return tex;
    }

    TextureRegion newTextureRegion(Texture& tex, int x, int y, int w, int h) {
        return TextureRegion(tex, x, y, w, h);
    }

    Model loadModel(const std::string &path)
    {
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;
        tinygltf::Model model;

        bool res = loader.LoadASCIIFromFile(&model, &err, &warn, path.c_str());
        if (!warn.empty()) Delog::warning("Loader: %s", warn.c_str());
        if (!err.empty()) Delog::error("Loader: %s", err.c_str());
        if (!res) 
            Delog::error("Loader: Failed to load %s", path.c_str());
        else
            Delog::msg("Loader: Successfully loaded %s", path.c_str());
        
        return Model(model);
    }
}
