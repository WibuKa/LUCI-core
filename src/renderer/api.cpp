#define DEFAULT_SHADER 3
#include "api.h"
#include "render_system.h"
#include "font_system.h"
#include "helper.h"
#include <stb/stb_image.h>
#include <utility>
#include "helper.h"

std::unordered_map<std::string,unsigned int> pathCache;
unsigned int shader_target = DEFAULT_SHADER;

GLuint font_default_id;
GLuint font_id;

namespace Render{

    float zoom = 1.0f;

    void setZoom(float value)
    {
        zoom = value;
    }

    void init(GLFWwindow* window){
        RenderSYS::init(window);
        font_default_id = FontSYS::load_font("FVF Fernando 08.ttf",15);
        font_id = font_default_id;
    }

    void set_Time(const float time){
        RenderSYS::setRunTime(time);
    }

    void resize(const int width,const int height){
        RenderSYS::resize(width,height);
    }

    void pushShader(unsigned int shader)
    {
        
    }

    unsigned int load_texture(const std::string &path,int cx,int cy)
    {
        unsigned int texID;
        if (pathCache.find(path) != pathCache.end()) return pathCache[path];

        try {
            texID = RenderSYS::createTexture(path,cx,cy);
            pathCache[path] = texID;
        }
        catch (const std::string& e) {
            std::cerr << "Error: " << e << std::endl;
            return 0;
        }

        return texID;
    }

    unsigned int get_texture_id(const std::string &path)
    {
        return load_texture(path,-1,-1);
    }

    std::tuple<int,int> getWindowSize()
    {
        return {RenderSYS::windowWidth,RenderSYS::windowHeight};
    }

    std::tuple<int,int> getTextureSize(unsigned id)
    {
        int width = 0;
        int height = 0;
        if(id) RenderSYS::get_textureSize(id,width,height);
        return {width,height};
    }

    void useShaderID(unsigned int ShaderID)
    {
        RenderSYS::useShader(ShaderID);
    }

    void set_font(int id)
    {
        FontSYS::set_font(id);
    }

    int load_font(std::string path, int font_size)
    {
        if (font_size < 0)
            font_size = 0;
        return FontSYS::load_font(path,font_size);
    }

    void drawText(const std::string& text, float x, float y, const std::string& align)
    {
        useShaderID(2);
        std::vector<uint32_t> unicodes = string2U32(text);

        struct Glyph {
            float x, y;
            float ox, oy;
            float w, h;
            int line;
        };

        std::vector<Glyph> glyphs;
        std::vector<float> lineWidths;

        float lineHeight = FontSYS::get_line_height();
        float cx = 0.0f, cy = 0.0f;
        uint32_t prev = 0;
        int lineIndex = 0;

        lineWidths.push_back(0.0f); // line 0 width


        // ========= BUILD GLYPH LIST =========
        for (uint32_t c : unicodes)
        {
            float ox, oy, w, h, asc, adv;
            FontSYS::get_glyph(c, ox, oy, w, h, asc, adv);

            if (c == '\n')
            {
                lineIndex++;
                lineWidths.push_back(0.0f);
                cx = 0;
                cy += lineHeight;
                prev = 0;
                continue;
            }

            float kern = FontSYS::get_kern(prev, c);

            // Store glyph position (not aligned yet)
            Glyph g;
            g.x = x + cx + adv * 0.5f;
            g.y = y + cy + h * 0.5f - asc + lineHeight;
            g.ox = ox; g.oy = oy;
            g.w = w;  g.h = h;
            g.line = lineIndex;

            glyphs.push_back(g);

            cx += adv + kern;
            lineWidths[lineIndex] = cx;
            prev = c;

        }

        // ========= PRE-CALC LINE OFFSETS =========
        std::vector<float> lineShift(lineWidths.size(), 0.0f);

        for (int i = 0; i < lineWidths.size(); i++)
        {
            if (align == "center")
                lineShift[i] = -lineWidths[i] * 0.5f;
            else if (align == "right")
                lineShift[i] = -lineWidths[i];
            else
                lineShift[i] = 0.0f;
        }

        // ========= RENDER =========
        for (const Glyph& g : glyphs)
        {
            float finalX = g.x + lineShift[g.line];
            RenderSYS::draw(
                1,
                finalX, g.y,
                1024, 1024,
                1, 1, 0,
                {g.ox, g.oy},
                {g.w, g.h}
            );
        }
    }


    void drawSquare(float x,float y,float width,float height,bool fill)
    {
        useShaderID(1);
        setUniformInt("geometry", fill ? 0 : 1);
        RenderSYS::add_quad(x,y,width,height,0);
    }


    void drawSprite(unsigned int id,float x,float y,float scale_x,float scale_y,float angle,int frame)
    {
        useShaderID(0);
        RenderSYS::add_sprite(id,x,y,scale_x,scale_y,angle,frame);
    }

    void setColor(float r,float g,float b,float a)
    {
        RenderSYS::setColor(r,g,b,a);
    }
    
    void setUniformBool(std::string name,bool v)
    {
        RenderSYS::setUniformInt(name,v);
    }
    void setUniformInt(std::string name,int v)
    {
        RenderSYS::setUniformInt(name,v);
    }
    void setUniformFloat(std::string name,float v)
    {
        RenderSYS::setUniformFloat(name,v);
    }
    void setUniformVec2(std::string name,float v1, float v2)
    {
        RenderSYS::setUniformVec2(name,v1,v2);
    }
    void setUniformVec3(std::string name,float v1, float v2,float v3)
    {
        RenderSYS::setUniformVec3(name,v1,v2,v3);
    }
    void setUniformVec4(std::string name,float v1, float v2, float v3, float v4)
    {
        RenderSYS::setUniformVec4(name,v1,v2,v3,v4);
    }
}