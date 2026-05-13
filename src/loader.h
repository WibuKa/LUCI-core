#pragma once
#include <string>
#include "sound.h"

namespace Loader {
    Sound load_sound(std::string path);
    Sound load_stream(std::string path);
}
