namespace ShaderArc{
constexpr const char* fragment_default_shader = R"(
#version 330 core

in vec2 UV;
in vec4 COLOR;
out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    FragColor = texture(uTexture, UV) * COLOR;
}
)";

constexpr const char* fragment_font_default_shader = R"(
#version 330 core

in vec2 UV;
in vec4 COLOR;
out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    float Smoothing = 0.5;
    float distance = texture(uTexture, UV).r;
    float alpha = smoothstep(0.5-Smoothing, 0.5+Smoothing, distance);
    FragColor = vec4(1.0, 1.0, 1.0, alpha) * COLOR;
}
)";

constexpr const char* fragment_geometry_shader = R"(
#version 330 core

in vec2 UV;
in vec4 COLOR;
out vec4 FragColor;

uniform vec2 uTextureSize;
uniform int uGeometry;

float lineWidth = 1.0;

void filledBox() {
    FragColor = COLOR;
}

void filledCircle() {
    vec2 d = UV - vec2(0.5);
    if (dot(d, d) < 0.25) 
        FragColor = COLOR;
    else 
        discard;
}

void lineBox() {
    vec2 p = UV * uTextureSize;
    bool isBorder = p.x < lineWidth || p.x > uTextureSize.x - lineWidth ||
                    p.y < lineWidth || p.y > uTextureSize.y - lineWidth;

    if (isBorder) 
        FragColor = COLOR;
    else 
        discard;
}

void lineCircle() {
    vec2 d = UV - vec2(0.5);
    float r2 = dot(d, d);
    float outer = 0.25;
    
    float thickness = lineWidth / min(uTextureSize.x, uTextureSize.y);
    float innerRadius = 0.5 - thickness;
    float inner = innerRadius * innerRadius;

    if (r2 < outer && r2 > inner) 
        FragColor = COLOR;
    else 
        discard;
}

void main() {
    switch(uGeometry) {
        case 0: filledBox();    break;
        case 1: filledCircle();  break;
        case 2: lineBox();       break;
        case 3: lineCircle();    break;
        default: discard;
    }
}
)";

constexpr const char* fragment_patch_shader = R"(
#version 330 core

in vec2 UV;
in vec4 COLOR;

out vec4 FragColor;

void main()
{
    FragColor = COLOR;
}
)";

constexpr const char* vertex_default_shader = R"(
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;

out vec2 UV;
out vec4 COLOR;

uniform vec2 uResolution;

void main()
{
    vec2 pos = aPos;

    pos.x = (pos.x / uResolution.x) * 2.0 - 1.0;
    pos.y = 1.0 - (pos.y / uResolution.y) * 2.0;

    gl_Position = vec4(pos, 0.0, 1.0);

    UV = aUV;
    COLOR = aColor;
}
)";


constexpr const char* fragment_default_shader_3D = R"(
#version 330 core

in vec2 UV;
out vec4 FragColor;
uniform sampler2D uTexture;
void main()
{
    FragColor = texture(uTexture, UV);
}

)";

constexpr const char* vertex_default_shader_3D = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 4) in ivec4 aBoneIDs;
layout(location = 5) in vec4 aWeights;

uniform mat4 uMVP;

out vec2 UV;
out vec3 NORMAL;
out vec4 BONEIDS;
out vec4 WEIGHTS;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
    UV = aUV;
    NORMAL = aNormal;
    BONEIDS = aBoneIDs;
    WEIGHTS = aWeights;
}

)";
}
