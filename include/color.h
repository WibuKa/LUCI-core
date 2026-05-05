#pragma once

struct Color {
    float r, g, b, a;
    Color(float _r = 0.0f, float _g = 0.0f, float _b = 0.0f, float _a = 0.0f) 
    : r(_r), g(_g), b(_b), a(_a) {}
};