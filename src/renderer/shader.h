#pragma once
#include "string"

class Shader
{
private:
public:
    void use(std::string path);
    void reset();
};

extern Shader shader;