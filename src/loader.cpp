#include "loader.h"
#include "audio.h"
#include "audio.h"
#include <memory>
#include <iostream>
#include <string>

namespace Loader {
    Sound load_sound(std::string path)
    {
        auto ptr = std::make_shared<ma_sound>();
        ma_result result = ma_sound_init_from_file(
            &Audio::engine,
            path.c_str(),
            0,
            NULL,
            NULL,
            ptr.get()
        );

        if (result != MA_SUCCESS) {
            std::cout << "Failed to load sound: " << path << "\n";
            return {};
        }
        return Sound(ptr, path, false);
    }

    Sound cloneSound(Sound& sound)
    {
        if(sound.isStream()){
            auto clone = std::make_shared<ma_sound>();
            std::string path = sound.getPath();
            if(ma_sound_init_from_file(
                &Audio::engine,
                path.c_str(),
                MA_SOUND_FLAG_STREAM,
                NULL,
                NULL,
                clone.get()
            ) != MA_SUCCESS){
                printf("Failed to clone audio\n");
                return {};
            }
            return Sound(clone,path,true);
        }
        else {
            auto clone = std::make_shared<ma_sound>();
            if(ma_sound_init_copy(&Audio::engine, 
                sound.get(), 
                0,
                nullptr,
                clone.get()
            ) != MA_SUCCESS){
                printf("Failed to clone audio\n");
                return {};
            }
            return Sound(clone,sound.getPath(),false);
        }
    }

    Sound load_stream(std::string path)
    {
        auto ptr = std::make_shared<ma_sound>();
        ma_result result = ma_sound_init_from_file(
            &Audio::engine,
            path.c_str(),
            MA_SOUND_FLAG_STREAM,
            NULL,
            NULL,
            ptr.get()
        );

        if (result != MA_SUCCESS) {
            std::cout << "Failed to load music: " << path << "\n";
            return {};
        }

        return Sound(ptr, path, true);
    }
}
