#include "texture.h"
#include "glad/glad.h"
#include <cstdio>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <stb/stb_image.h>

std::unordered_map<std::string, size_t> textures_cache;
std::vector<std::unique_ptr<Texture>> loaded_textures;

Texture create_texture(const std::string &path) {
    int width, height, channel;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channel, 0);

    if (data == nullptr) throw std::runtime_error("Failed to load: " + path);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum internalFormat = GL_RGB;
    GLenum format = GL_RGB;

    if (channel == 1) {
        internalFormat = GL_RED;
        format = GL_RED;
    } else if (channel == 2) {
        internalFormat = GL_RG;
        format = GL_RG;
    } else if (channel == 3) {
        internalFormat = GL_RGB;
        format = GL_RGB;
    } else if (channel == 4) {
        internalFormat = GL_RGBA;
        format = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    return Texture(textureID, width, height);
}

Texture* load_texture(const std::string& path)
{
    auto it = textures_cache.find(path);
    if (it != textures_cache.end()) {
        return loaded_textures[it->second].get();}

    size_t index = loaded_textures.size();
    loaded_textures.push_back(std::make_unique<Texture>(create_texture(path)));
    textures_cache[path] = index;
    return loaded_textures.back().get();
}
