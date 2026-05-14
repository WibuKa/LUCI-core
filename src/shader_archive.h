namespace ShaderArc{
constexpr const char* fragment_default_shader = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;

    uniform sampler2D uTexture;
    uniform vec4 uColor;

    uniform vec2 uTextureSize;
    uniform vec4 uRegion;

    void main()
    {
        vec2 UV = TexCoord * uRegion.zw/uTextureSize + uRegion.xy/uTextureSize;
        FragColor = texture(uTexture, UV) * uColor;
    }

)";

constexpr const char* fragment_font_default_shader = R"(
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

constexpr const char* fragment_geometry_shader = R"(
#version 330 core

    out vec4 FragColor;
    in vec2 TexCoord;

    uniform vec2 uTextureSize;
    uniform vec4 uColor;
    uniform int geometry;
    float lineWidth = 1.0;

    void filledBox()
    {
        FragColor = uColor;
    }

    void filledCircle()
    {
        vec2 d = TexCoord - vec2(0.5);

        FragColor = (dot(d, d) < 0.25)
            ? uColor
            : vec4(0.0);
    }

    void lineBox()
    {
        vec2 p = TexCoord * uTextureSize;

        bool left   = p.x < lineWidth;
        bool right  = p.x > uTextureSize.x - lineWidth;

        bool top    = p.y < lineWidth;
        bool bottom = p.y > uTextureSize.y - lineWidth;

        FragColor = (left || right || top || bottom)
            ? uColor
            : vec4(0.0);
    }

    void lineCircle()
    {
        vec2 d = TexCoord - vec2(0.5);

        float r2 = dot(d, d);

        float outer = 0.25;
        
        float innerRadius = 0.5 - (lineWidth / min(uTextureSize.x, uTextureSize.y));
        float inner = innerRadius * innerRadius;

        FragColor = (r2 < outer && r2 > inner)
            ? uColor
            : vec4(0.0);
    }

    void main()
    {
        switch(geometry)
        {
            case 0:
                filledBox();
                break;
            case 1:
                filledCircle();
                break;
            case 2:
                lineBox();
                break;
            case 3:
                lineCircle();
                break;
        }
    }
)";




constexpr const char* vertex_default_shader = R"(
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

uniform vec2 uResolution;
uniform vec2 uTextureSize;
uniform vec2 uPos;
uniform vec2 uScale;
uniform vec2 uView;
uniform vec4 uRegion;
uniform float uAngle;
uniform float uTime;

out vec2 TexCoord;

void main()
{
    TexCoord = aUV;

    // local space
    vec2 pos = aPos * uRegion.zw * uScale;

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

constexpr const char* fragment_patch_shader = R"(
#version 330 core

in vec2 vUV;
in vec4 vColor;

out vec4 FragColor;

void main()
{
    FragColor = vColor;
}
)";

constexpr const char* vertex_patch_shader = R"(
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;

out vec2 vUV;
out vec4 vColor;

uniform vec2 uResolution;

void main()
{
    vec2 pos = aPos;

    pos.x = (pos.x / uResolution.x) * 2.0 - 1.0;
    pos.y = 1.0 - (pos.y / uResolution.y) * 2.0;

    gl_Position = vec4(pos, 0.0, 1.0);

    vUV = aUV;
    vColor = aColor;
}
)";
}
