#include "shader.h"
#include "unordered_map"

Shader shader;
std::unordered_map<std::string,unsigned int> shaderCache;

void Shader::use(std::string path)
{
    if (shaderCache.find(path) == shaderCache.end())
    {
        unsigned int shaderID = 0;
        //renderSystem.
        shaderCache[path] = shaderID;
    }
}
