#pragma once
#include <miniaudio.h>
#include "sound.h"

namespace Audio{
    extern ma_engine engine;
    void init();
    void play(Sound& sound);
    void stop(Sound& sound);
    void pause(Sound& sound);
    void emitSound(Sound& sound);
    void close(); 
}
