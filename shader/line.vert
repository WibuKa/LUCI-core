#version 330 core
layout(location = 0) in vec2 aPos;

uniform vec2 start; 
uniform vec2 end;  

void main()
{
   vec2 dir = end - start;
   float len = length(dir);
   float angle = atan(dir.y, dir.x);

   mat2 rot = mat2(cos(angle), -sin(angle),sin(angle),  cos(angle));

   vec2 pos = rot * (aPos * len) + start;

   gl_Position = vec4(pos, 0.0, 1.0);
}