#define MINIAUDIO_IMPLEMENTATION
#include "audio.h"
#include "vector"
#include <iostream>
#include <filesystem>
#include <memory>

namespace Audio{
    ma_engine engine;
    std::vector<std::unique_ptr<ma_sound>> sound_data;
    std::vector<std::unique_ptr<ma_sound>> clones;

    void init() {
        ma_result res = ma_engine_init(NULL, &engine);
        if (res != MA_SUCCESS) {
            std::cout << "Engine init failed: " << res << "\n";
            return;
        }
        ma_engine_start(&engine);
    }

    unsigned int get_sound_id(std::string path)
    {
        if (!std::filesystem::exists(path)) {
            std::cout << "File not found: " << path << "\n";
        }
        ma_result result;
        auto sound = std::make_unique<ma_sound>();
        result = ma_sound_init_from_file(&engine, path.c_str(), 0, NULL, NULL, sound.get());
        if (result != MA_SUCCESS) {
            std::cout << "Load sound failed: " << path << " err=" << result << "\n";
            return 0;
        }
        sound_data.push_back(std::move(sound));
        return sound_data.size();
    }

    void clean_up()
    {
        for (auto it = clones.begin(); it != clones.end();){
            ma_sound* snd = it->get();
            if (ma_sound_is_playing(snd)){
                ++it;
                continue;
            }
            ma_sound_uninit(snd);
            it = clones.erase(it); 
        }
    }

    void play(unsigned int id, float volum, float pitch)
    {
        if(id == 0 || id > sound_data.size()) return;
        auto clone = std::make_unique<ma_sound>();
        if(ma_sound_init_copy(&engine, sound_data[id - 1].get(), 0, nullptr, clone.get()) != MA_SUCCESS) return;
        ma_sound_set_volume(clone.get(),volum);
        ma_sound_set_pitch(clone.get(),pitch);
        ma_sound_start(clone.get());
        clones.push_back(std::move(clone));
        clean_up();
    }

    void loop(unsigned int id, float volum, float pitch)
    {
        if(id == 0 || id > sound_data.size()) return;
        
        auto clone = std::make_unique<ma_sound>();
        if(ma_sound_init_copy(&engine, sound_data[id - 1].get(), 0, nullptr, clone.get()) != MA_SUCCESS) return;
        
        ma_sound_set_volume(clone.get(), volum);
        ma_sound_set_pitch(clone.get(), pitch);
        
        // SET LOOP FLAG - QUAN TRỌNG
        ma_sound_set_looping(clone.get(), MA_TRUE);
        
        ma_sound_start(clone.get());
        clones.push_back(std::move(clone));
        clean_up();
    }

    void update(){
        
    }

    void close(){
        ma_engine_uninit(&engine);
    }
}
