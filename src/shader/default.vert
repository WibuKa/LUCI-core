#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 uPos;
uniform vec2 uScale;
uniform vec2 crop;
uniform vec2 pixelSize;

void main()
{
   vec2 pos = (aPos * uScale * crop + uPos * pixelSize);
   gl_Position = vec4(pos, 0.0, 1.0);
   TexCoord = aTexCoord;
}
