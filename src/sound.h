#pragma once
#include <miniaudio.h>
#include <memory>
#include <string>
#include <utility>

struct ma_sound;

class Sound {
    private:
        float volume = 1.0f;
        float pitch = 1.0f;
        bool looping = false;
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

        std::string getPath(){
            return path;
        }

        bool isStream(){
            return stream;
        }

        void setVolume(float _volume){
            ma_sound_set_volume(sound.get(),_volume);
            volume = _volume;
        }

        float getVolume(){
            return volume;
        }

        void setLooping(bool _looping){
            if(_looping)
                ma_sound_set_looping(sound.get(), MA_TRUE);
            else
                ma_sound_set_looping(sound.get(), MA_FALSE);
            looping = _looping;
        }
        
        bool getLooping(){
            return looping;
        }

        void setPitch(float _pitch){
            ma_sound_set_volume(sound.get(),_pitch);
            pitch = _pitch;
        }

        float getPitch(){
            return pitch;
        }

        std::string type(){
            return stream ? "music" : "sound";
        }
};
