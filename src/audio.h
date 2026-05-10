#pragma once
#include <miniaudio.h>
#include <string>

namespace Audio{
    void init();
    unsigned int get_sound_id(std::string path);
    void loop(unsigned int id, float volum, float pitch);
    void play(unsigned int id, float volum, float pitch);
    void close(); 
}
