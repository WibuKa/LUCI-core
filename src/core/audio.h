#include "miniaudio.h"
#include <string>

namespace Audio{
    extern ma_engine engine;
    void init();
    unsigned int get_sound_id(std::string path);
    void loop(unsigned int id, float volum, float pitch);
    void play(unsigned int id, float volum, float pitch);
}
