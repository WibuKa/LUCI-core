#define MINIAUDIO_IMPLEMENTATION
#include "audio.h"
#include "vector"
#include "delog.h"
#include <memory>

namespace Audio{
    ma_engine engine;
    std::vector<std::unique_ptr<ma_sound>> sound_clones;

    void init() {
        ma_result res = ma_engine_init(NULL, &engine);
        if (res != MA_SUCCESS) {
            Delog::msg("Engine init failed: %d", res);
            return;
        }
        ma_engine_start(&engine);
    }

    void cleanUpClones()
    {
        for (auto it = sound_clones.begin(); it != sound_clones.end();){
            ma_sound* snd = it->get();
            if (ma_sound_is_playing(snd)){
                ++it;
                continue;
            }
            ma_sound_uninit(snd);
            it = sound_clones.erase(it); 
        }
    }

    void play(Sound& sound)
    {
        if(ma_sound_is_playing(sound.get()))ma_sound_seek_to_pcm_frame(sound.get(), 0);
        ma_sound_start(sound.get());
    }

    void stop(Sound& sound)
    {
        ma_sound_seek_to_pcm_frame(sound.get(), 0);
        ma_sound_stop(sound.get());
    }

    void pause(Sound& sound)
    {
        ma_sound_stop(sound.get());
    }

    void emitSound(Sound& sound)
    {
        if(sound.isStream()){
            Delog::msg("Cannot emit music");
            return;
        }

        auto clone = std::make_unique<ma_sound>();
        if(ma_sound_init_copy(&engine, sound.get(), 0, nullptr, clone.get()) != MA_SUCCESS){
            Delog::msg("Failed to play sound");
            return;
        }
        ma_sound_set_volume(clone.get(),sound.getVolume());
        ma_sound_set_pitch(clone.get(),sound.getPitch());
        ma_sound_start(clone.get());
        sound_clones.push_back(std::move(clone));
        cleanUpClones();
    }

    void loop(Sound sound, float volum, float pitch)
    {
        cleanUpClones();
    }

    void close(){
        ma_engine_uninit(&engine);
    }
}
