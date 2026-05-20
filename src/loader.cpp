#include "loader.h"
#include <memory>
#include <iostream>
#include <string>
#include <stb/stb_image.h>
#include <unordered_map>
#include <vector>
#include <fstream>
#include "audio.h"
#include "system.h"
#include "texture.h"
#include "font.h"
#include "renderer.h"

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

        if (!defaultFont.isValid())
        {
            defaultFont = loadFont("font.ttf", 16); 
        }

        Render::setFont(defaultFont);
    }

    std::vector<int> getCodepoints(stbtt_fontinfo* font)
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
    
    Font loadFont(const std::string& path,float font_size)
    {
        stbtt_fontinfo fontInfo = {};
        
        int font_atlas_width = 1024;
        int font_atlas_height = 1024;
        
        std::ifstream inputStream(path.c_str(), std::ios::binary);
        if (!inputStream) 
        {
            std::cerr << "Failed to open font file: " << path << "\n";
            return {};
        }

        inputStream.seekg(0, std::ios::end);
        size_t pathSize = inputStream.tellg();
        inputStream.seekg(0, std::ios::beg);

        std::vector<uint8_t> fontDataBuf(pathSize);
        std::vector<uint8_t> fontAtlasTextureData(font_atlas_width * font_atlas_height, 0);
        
        inputStream.read((char*)fontDataBuf.data(), pathSize);

        if (!stbtt_InitFont(&fontInfo, fontDataBuf.data(), 0))
        {
            std::cerr << "stbtt_InitFont() Failed!\n";
            return {};
        } 
        
        std::vector<int> codepoints = getCodepoints(&fontInfo);
        std::vector<stbtt_packedchar> packedChars(codepoints.size());

        stbtt_pack_context ctx;
        stbtt_PackBegin(&ctx, fontAtlasTextureData.data(), font_atlas_width, font_atlas_height, 0, 1, nullptr);
        

        stbtt_pack_range range;
        range.first_unicode_codepoint_in_range   = 0;
        range.font_size                          = font_size;
        range.num_chars                          = codepoints.size();
        range.chardata_for_range                 = packedChars.data();
        range.array_of_unicode_codepoints        = codepoints.data();

        if (!stbtt_PackFontRanges(&ctx, fontDataBuf.data(), 0, &range, 1))
        {
            std::cerr << "Failed to pack font\n";
            stbtt_PackEnd(&ctx);
            return {};
        }

        float scale_for_pixel = stbtt_ScaleForPixelHeight(&fontInfo, font_size);
        int ascent, descent, line_gap;
        stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &line_gap);
        int line_height = (ascent - descent + line_gap) * scale_for_pixel;
        
        std::unordered_map<uint32_t, Glyph> glyph_map;

        for (size_t i = 0; i < codepoints.size(); i++)
        {
            uint32_t c = codepoints[i];
            const auto& pc = packedChars[i];

            glyph_map.emplace(c, Glyph{
                c,

                pc.x1 - pc.x0,
                pc.y1 - pc.y0,

                pc.xoff,
                pc.yoff,

                (float)pc.xadvance,

                (float)pc.x0 / font_atlas_width,
                (float)pc.y0 / font_atlas_height,

                (float)pc.x1 / font_atlas_width,
                (float)pc.y1 / font_atlas_height
            });
        }

        stbtt_PackEnd(&ctx);

        Texture atlas_texture = Render::createTexture(fontAtlasTextureData.data(), font_atlas_width, font_atlas_height, 1);

        return Font(
            true,
            path, 
            atlas_texture, 
            1.0,
            ascent,
            descent, 
            line_gap, 
            line_height, 
            0.0, 
            std::make_shared<std::unordered_map<uint32_t,Glyph>>(glyph_map)
        );
    }

    Sound load_sound(std::string path)
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
            std::cout << "Failed to load sound: " << path << "\n";
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
                printf("Failed to clone audio\n");
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
                printf("Failed to clone audio\n");
                return {};
            }
            return Sound(clone,sound.getPath(),false);
        }
    }

    Sound load_stream(std::string path)
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
            std::cout << "Failed to load music: " << path << "\n";
            return {};
        }

        return Sound(ptr, path, true);
 
    }


    Texture load_texture(const std::string& path){
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

    TextureRegion new_texture_region(Texture& tex, int x, int y, int w, int h) {
        printf("new_texture_region\n");
        return TextureRegion(tex, x, y, w, h);
    }
}
