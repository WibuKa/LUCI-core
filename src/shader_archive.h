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
constexpr const char* fragment_default_3D_shader = R"(
#version 330 core

#define MAX_LIGHTS 4
#define MAX_TEXTURES 5

#define BASE_COLOR_TEXTURE_SLOT 0
#define NORMAL_TEXTURE_SLOT 1
#define METALLIC_TEXTURE_SLOT 2
#define OCCLUSION_TEXTURE_SLOT 3
#define EMISSIVE_TEXTURE_SLOT 4

in vec2 UV;
in vec3 NORMAL;
in vec3 FragPos;

out vec4 FragColor;

struct Light
{
    int type;
    
    // Common
    vec3 position;
    vec3 color;

    float intensity;
    float distance;

    // Directional
    vec3 direction;

    // Spot
    float spotAngle;
    float spotExponent;
};

uniform int uLightCount;
uniform Light uLight[MAX_LIGHTS];

uniform vec3 uViewPos;

// Uniforms kiểm tra sự tồn tại của từng loại texture (Gửi từ CPU: true/false)
uniform bool uHasBaseColorMap;
uniform bool uHasNormalMap;
uniform bool uHasMetallicMap;
uniform bool uHasOcclusionMap;
uniform bool uHasEmissiveMap;

uniform sampler2D uTextures[MAX_TEXTURES];

void main()
{
    // ---------------- LẤY DỮ LIỆU TỪ TEXTURES (CÓ FALLBACK) ----------------
    
    // 1. Base Color (Nếu không có, mặc định là màu trắng vec3(1.0))
    vec3 baseColor = uHasBaseColorMap ? texture(uTextures[BASE_COLOR_TEXTURE_SLOT], UV).rgb : vec3(1.0);
    
    // 2. Normal Map (Nếu không có, dùng Normal mặc định từ Vertex Shader)
    vec3 norm = normalize(NORMAL);
    if(uHasNormalMap)
    {
        // Ghi chú: Để Normal Map hoạt động chuẩn xác 100%, bạn nên truyền thêm vector Tangent từ C++ 
        // và tính ma trận TBN. Đoạn code dưới đây là tính toán cơ bản trong không gian Tangent/Local.
        vec3 normalMap = texture(uTextures[NORMAL_TEXTURE_SLOT], UV).rgb;
        normalMap = normalize(normalMap * 2.0 - 1.0); // Chuyển từ khoảng [0, 1] sang [-1, 1]
        // Tạm thời hòa trộn nhẹ hoặc giả định normalMap áp dụng trực tiếp (tốt nhất là dùng TBN)
        norm = normalize(norm + normalMap * 0.5); 
    }

    // 3. Metallic (Nếu không có, mặc định độ phản xạ kim loại bằng 0.0)
    float metallic = uHasMetallicMap ? texture(uTextures[METALLIC_TEXTURE_SLOT], UV).r : 0.0;
    
    // 4. Ambient Occlusion (Nếu không có, mặc định không che khuất = 1.0)
    float ao = uHasOcclusionMap ? texture(uTextures[OCCLUSION_TEXTURE_SLOT], UV).r : 1.0;
    
    // 5. Emissive (Tự phát sáng, nếu không có mặc định bằng vec3(0.0))
    vec3 emissive = uHasEmissiveMap ? texture(uTextures[EMISSIVE_TEXTURE_SLOT], UV).rgb : vec3(0.0);

    // ---------------- TÍNH TOÁN ÁNH SÁNG ----------------
    
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 lightResult = vec3(0.0);

    // Ambient (Hòa trộn với Ambient Occlusion)
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * vec3(1.0, 0.95, 0.8) * ao;

    for(int i = 0; i < uLightCount; i++)
    {
        Light l = uLight[i];
        vec3 lightDir;
        float attenuation = 1.0;

        // ---------------- Directional ----------------
        if(l.type == 0)
        {
            lightDir = normalize(-l.direction);
        }
        // ---------------- Point ----------------
        else if(l.type == 1)
        {
            vec3 toLight = l.position - FragPos;
            float dist = length(toLight);
            lightDir = normalize(toLight);

            attenuation = 1.0 / (dist * dist + 0.0001);
            float windowing = max(1.0 - (dist / l.distance), 0.0);
            windowing *= windowing;
            attenuation *= windowing;
        }
        // ---------------- Spot ----------------
        else if(l.type == 2)
        {
            vec3 toLight = l.position - FragPos;
            float dist = length(toLight);
            lightDir = normalize(toLight);

            float theta = dot(lightDir, normalize(-l.direction));
            float cutoff = cos(radians(l.spotAngle));

            if(theta > cutoff)
            {
                float spotFactor = pow(theta, l.spotExponent);
                attenuation = spotFactor / (dist * dist + 0.0001);
            }
            else
            {
                attenuation = 0.0;
            }
        }

        // Diffuse
        float diff = max(dot(norm, lightDir), 0.0);

        // Blinn-Phong Specular (Tận dụng giá trị Metallic để tăng độ bóng)
        vec3 halfwayDir = normalize(lightDir + viewDir);
        // Nếu là kim loại (metallic cao), tăng độ phản xạ lấp lánh (Shininess)
        float shininess = mix(64.0, 256.0, metallic); 
        float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);

        vec3 uLightPower = l.color * l.intensity * attenuation;

        vec3 diffuse = diff * uLightPower;
        // Kim loại sẽ phản xạ nhiều ánh sáng specular hơn
        vec3 specular = spec * uLightPower * mix(0.2, 1.0, metallic); 

        lightResult += diffuse + specular;
    }

    // ---------------- KẾT QUẢ CUỐI CÙNG ----------------
    // Màu tổng hợp = (Ambient + Ánh sáng đèn) * BaseColor từ Texture + Màu tự phát sáng (Emissive)
    vec3 finalColor = (ambient + lightResult) * baseColor + emissive;

    FragColor = vec4(finalColor, 1.0);
})";
 

/*
constexpr const char* fragment_default_3D_shader = R"(
#version 330 core

in vec3 NORMAL;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 uLightPos;
uniform vec3 uLightColor;

uniform vec3 uViewPos;
uniform vec3 uObjectColor;

void main()
{
    vec3 norm = normalize(NORMAL);

    vec3 lightDir = normalize(uLightPos - FragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);

    // specular
    vec3 viewDir = normalize(uViewPos - FragPos);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);

    // attenuation
    float dist = length(uLightPos - FragPos);

    float attenuation = 1.0 / (dist * dist + 1.0);

    vec3 ambient = vec3(0.1);

    vec3 diffuse = diff * uLightColor;

    vec3 specular = spec * uLightColor * 0.5;

    vec3 result =
        ambient +
        (diffuse + specular) * attenuation;

    FragColor = vec4(result * uObjectColor, 1.0);
})";
*/
constexpr const char* vertex_default_3D_shader = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;
layout(location = 4) in ivec4 aBoneIDs;
layout(location = 5) in vec4 aWeights;

uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uView;

out vec2 UV;
out vec3 NORMAL;
out vec3 FragPos;

void main()
{
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    NORMAL = mat3(transpose(inverse(uModel))) * aNormal;
    UV = aUV;
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}

)";
}
