#version 330 core
in vec2 vTexCoord;     // UV từ vertex
out vec4 FragColor;

uniform sampler2D tex;
uniform vec2 texSize;  // (width, height) của texture
uniform float angle;   // góc xoay radian
uniform vec2 center;   // tâm xoay (0..1 UV)

void main()
{
    vec2 uv = vTexCoord - center;

    mat2 rot = mat2(cos(angle), -sin(angle),
                    sin(angle),  cos(angle));
    uv = rot * uv;

    uv += center;

    uv = floor(uv * texSize + 0.5) / texSize;

    FragColor = texture(tex, uv);
}
