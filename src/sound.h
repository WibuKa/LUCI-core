#pragma once
#include <miniaudio.h>
#include <memory>
#include <string>
#include <utility>

struct ma_sound;

class Sound {
    private:
        std::shared_ptr<ma_sound> sound;
        std::string path;
        bool stream;
    public:
        Sound() = default;

        Sound(std::shared_ptr<ma_sound> _sound,const std::string& _path, bool _stream){
            path = _path;
            sound = std::move(_sound);
            stream = _stream;
        }

        void set(std::shared_ptr<ma_sound> sound){
            sound = std::move(sound);
        }

        ma_sound* get(){
            return sound.get();
        }

        bool isStream(){
            return stream;
        }

        std::string type(){
            return stream ? "music" : "sound";
        }
};
