#include "resource.h"
#include "file.h"
#include "renderer/api.h"
#include "core/audio.h"

namespace Resource
{

    unsigned int load_image(const std::string& path)
    {
        return Render::get_texture_id(path);
    }

    unsigned int load_audio(const std::string& path)
    {
        return Audio::get_sound_id(path);
    }

    unsigned int load(const std::string& path)
    {
        std::string ext = File::get_ext(path);
        if(ext == ".png")
            return load_image(path);
        if(ext == ".wav" || ext == ".mp3" || ext == ".ogg")
            return load_audio(path);

        return 0;
    }
}