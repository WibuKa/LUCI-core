#include "loader.h"
#include "audio.h"
#include "audio.h"
#include <memory>
#include <iostream>

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
