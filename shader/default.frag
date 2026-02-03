#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture;
uniform vec2 coord;
uniform bool visible;
uniform vec2 frame;

uniform bool debug;

void main()
{

    vec2 UV = TexCoord;
    vec2 size = vec2(1.0,1.0)/coord;
    UV = size * (frame + UV);

    FragColor = texture(texture, UV);

}