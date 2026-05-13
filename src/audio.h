#pragma once
#include <miniaudio.h>
#include "sound.h"

namespace Audio{
    extern ma_engine engine;
    void init();
    void loop(Sound sound, float volum, float pitch);
    void play(Sound& sound, float volum, float pitch);
    void emitSound(Sound& sound, float volum, float pitch);
    void close(); 
}
