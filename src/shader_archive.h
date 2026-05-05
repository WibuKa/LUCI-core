#include <string>

namespace ShaderArc{
std::string fragment_default_shader = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;

    uniform sampler2D uTexture;
    uniform vec4 uColor;

    uniform vec2 uTextureSize;
    uniform vec2 uFrameOffset;
    uniform vec2 uFrameSize;

    void main()
    {
        vec2 frameUV = TexCoord * uFrameSize/uTextureSize + uFrameOffset/uTextureSize;

        FragColor = texture(uTexture, frameUV) * uColor;
    }

)";

std::string fragment_font_default_shader = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;

    uniform sampler2D uTexture;
    uniform vec4 uColor;

    uniform vec2 uTextureSize;
    uniform vec2 uFrameOffset;
    uniform vec2 uFrameSize;

    void main()
    {
        vec2 frameUV = TexCoord * uFrameSize/uTextureSize + uFrameOffset/uTextureSize;
        float Smoothing = 0.5;
        float distance = texture(uTexture, frameUV).r;
        float alpha = smoothstep(0.5-Smoothing, 0.5+Smoothing, distance);
        FragColor = vec4(1.0, 1.0, 1.0, alpha) * uColor;
    }
)";

std::string fragment_geometry_shader = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;

    uniform vec2 uTextureSize;
    uniform int geometry;
    uniform vec4 uColor;

    vec4 color;
    void lineBox(){
        vec2 uv = TexCoord * uTextureSize + vec2(0,0.1);
        float line = 1.0;

        bool borderX = (uv.x < line) || (uv.x > uTextureSize.x - line);
        bool borderY = (uv.y < line) || (uv.y > uTextureSize.y - line);    
        if (borderX || borderY)
            color = uColor;
        else
            color = vec4(0.0, 0.0, 0.0, 0.0);
    }

    void main() {
        if(geometry == 0){
            color = uColor;
        }
        else{
            lineBox();
        }
        FragColor = color;
    }

)";

std::string vertex_default_shader = R"(
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

uniform vec2 uResolution;
uniform vec2 uTextureSize;
uniform vec2 uFrameSize;
uniform vec2 uPos;
uniform vec2 uScale;
uniform vec2 uView;
uniform float uAngle;
uniform float uTime;

out vec2 TexCoord;

void main()
{
    TexCoord = aUV;

    // local space
    vec2 pos = aPos * uFrameSize * uScale;

    // rotation
    float c = cos(uAngle);
    float s = sin(uAngle);
    pos = mat2(c, -s, s, c) * pos;

    pos -= uPos * vec2(-2.0, 2.0);
    pos -= uView;
    vec2 clipPos = pos / uResolution;
    clipPos.y = -clipPos.y;

    gl_Position = vec4(clipPos,0.0, 1.0);
}
)";
}