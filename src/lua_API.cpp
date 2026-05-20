#include "lua_API.h"
#include "input_system.h"
#include "lua.h"
#include "renderer.h"
#include "window.h"
#include "audio.h"
#include "sol/table.hpp"
#include "texture.h"
#include "texture_region.h"
#include "loader.h"
#include "timer.h"
#include <cstdio>

namespace Lua{
    sol::state lua;
    sol::function load;
    sol::function update;
    sol::function draw;
    
    sol::table luci     = lua.create_table();
    sol::table window   = lua.create_table();
    sol::table texture  = lua.create_table();
    sol::table audio    = lua.create_table();
    sol::table graphics = lua.create_table();
    sol::table source   = lua.create_table();
    sol::table input    = lua.create_table();
    sol::table config   = lua.create_table();

    void luci_API()
    {
        lua["luci"]       = luci;
        luci["window"]    = window;
        luci["texture"]   = texture;
        luci["source"]    = source;
        luci["graphics"]  = graphics;
        luci["audio"]     = audio;
        luci["input"]     = input;
        luci["config"]     = config;
       
        config.set_function("set_max_framerate",&Time::set_max_framerate);
    }

    void loader_API()
    {
        source.set_function("new_texture_region",&Loader::new_texture_region);
        source.set_function("load_image",&Loader::load_texture);
        source.set_function("clone_audio",&Loader::cloneSound);
        source.set_function("load_shader",&Render::loadShader);
        source.set_function("load_sound",&Loader::load_sound);   
        source.set_function("load_music",&Loader::load_stream);
    }

    void window_API()
    {
        window.set_function("set_resizable", &Window::set_resizable);
        window.set_function("set_fullscreen", &Window::set_fullscreen);
        window.set_function("set_windowed", &Window::set_windowed);
        window.set_function("set_borderless_windowed", &Window::set_borderless_windowed);
        window.set_function("get_width", &Window::get_width);
        window.set_function("get_height", &Window::get_height);
    }

    void draw_API()
    {
        lua.set_function("set_view_translate",&Render::setViewTranslate);
        lua.set_function("get_window_size",&Render::getWindowSize);
        lua.set_function("get_texture_size",&Render::getTextureSize);
        lua.set_function("set_font",&Render::setFont);

        lua.set_function("set_color",&Render::setColor);

        lua.new_usertype<Texture>("Texture",
            "width", &Texture::getWidth,
            "height",&Texture::getHeight
        );

        lua.new_usertype<TextureRegion>("TextureRegion",
            "set_texture", &TextureRegion::set_texture,
            "set_region", &TextureRegion::set_region
        );

        graphics.set_function("draw_sprite",&Render::drawSprite);
        graphics.set_function("draw_rectangle",&Render::drawRectangle);
        graphics.set_function("draw_circle",&Render::drawCircle);
        graphics.set_function("draw_text",&Render::drawText);
        graphics.set_function("set_shader",&Render::setShader);
    }

    void input_API()
    {
        input.set_function("is_down",&Input::isKeyDown);
        input.set_function("is_press",&Input::isKeyPress);
        input.set_function("is_released",&Input::isKeyReleased);

        sol::table keyboard = lua.create_table();
        keyboard.set_function("is_down",&Input::isKeyboardDown);
        keyboard.set_function("is_press",&Input::isKeyboardPress);
        keyboard.set_function("is_released",&Input::isKeyboardReleased);
        input["keyboard"] = keyboard;

        sol::table mouse = lua.create_table();
        mouse.set_function("is_down",&Input::isMouseDown);
        mouse.set_function("is_press",&Input::isMousePress);
        mouse.set_function("is_released",&Input::isMouseReleased);
        mouse.set_function("get_position",&Input::getMousePos);
        input["mouse"] = mouse;

        // --- Numbers ---
        input["KEY_0"] = GLFW_KEY_0;
        input["KEY_1"] = GLFW_KEY_1;
        input["KEY_2"] = GLFW_KEY_2;
        input["KEY_3"] = GLFW_KEY_3;
        input["KEY_4"] = GLFW_KEY_4;
        input["KEY_5"] = GLFW_KEY_5;
        input["KEY_6"] = GLFW_KEY_6;
        input["KEY_7"] = GLFW_KEY_7;
        input["KEY_8"] = GLFW_KEY_8;
        input["KEY_9"] = GLFW_KEY_9;

        // --- Alphabet ---
        input["KEY_A"] = GLFW_KEY_A;
        input["KEY_B"] = GLFW_KEY_B;
        input["KEY_C"] = GLFW_KEY_C;
        input["KEY_D"] = GLFW_KEY_D;
        input["KEY_E"] = GLFW_KEY_E;
        input["KEY_F"] = GLFW_KEY_F;
        input["KEY_G"] = GLFW_KEY_G;
        input["KEY_H"] = GLFW_KEY_H;
        input["KEY_I"] = GLFW_KEY_I;
        input["KEY_J"] = GLFW_KEY_J;
        input["KEY_K"] = GLFW_KEY_K;
        input["KEY_L"] = GLFW_KEY_L;
        input["KEY_M"] = GLFW_KEY_M;
        input["KEY_N"] = GLFW_KEY_N;
        input["KEY_O"] = GLFW_KEY_O;
        input["KEY_P"] = GLFW_KEY_P;
        input["KEY_Q"] = GLFW_KEY_Q;
        input["KEY_R"] = GLFW_KEY_R;
        input["KEY_S"] = GLFW_KEY_S;
        input["KEY_T"] = GLFW_KEY_T;
        input["KEY_U"] = GLFW_KEY_U;
        input["KEY_V"] = GLFW_KEY_V;
        input["KEY_W"] = GLFW_KEY_W;
        input["KEY_X"] = GLFW_KEY_X;
        input["KEY_Y"] = GLFW_KEY_Y;
        input["KEY_Z"] = GLFW_KEY_Z;

        // --- Symbols ---
        input["KEY_SPACE"] = GLFW_KEY_SPACE;
        input["KEY_APOSTROPHE"] = GLFW_KEY_APOSTROPHE;
        input["KEY_COMMA"] = GLFW_KEY_COMMA;
        input["KEY_MINUS"] = GLFW_KEY_MINUS;
        input["KEY_PERIOD"] = GLFW_KEY_PERIOD;
        input["KEY_SLASH"] = GLFW_KEY_SLASH;
        input["KEY_SEMICOLON"] = GLFW_KEY_SEMICOLON;
        input["KEY_EQUAL"] = GLFW_KEY_EQUAL;
        input["KEY_LEFT_BRACKET"] = GLFW_KEY_LEFT_BRACKET;
        input["KEY_BACKSLASH"] = GLFW_KEY_BACKSLASH;
        input["KEY_RIGHT_BRACKET"] = GLFW_KEY_RIGHT_BRACKET;
        input["KEY_GRAVE_ACCENT"] = GLFW_KEY_GRAVE_ACCENT;

        // --- Function keys ---
        input["KEY_ESCAPE"] = GLFW_KEY_ESCAPE;
        input["KEY_ENTER"] = GLFW_KEY_ENTER;
        input["KEY_TAB"] = GLFW_KEY_TAB;
        input["KEY_BACKSPACE"] = GLFW_KEY_BACKSPACE;
        input["KEY_INSERT"] = GLFW_KEY_INSERT;
        input["KEY_DELETE"] = GLFW_KEY_DELETE;
        input["KEY_RIGHT"] = GLFW_KEY_RIGHT;
        input["KEY_LEFT"] = GLFW_KEY_LEFT;
        input["KEY_DOWN"] = GLFW_KEY_DOWN;
        input["KEY_UP"] = GLFW_KEY_UP;
        input["KEY_PAGE_UP"] = GLFW_KEY_PAGE_UP;
        input["KEY_PAGE_DOWN"] = GLFW_KEY_PAGE_DOWN;
        input["KEY_HOME"] = GLFW_KEY_HOME;
        input["KEY_END"] = GLFW_KEY_END;

        input["KEY_CAPS_LOCK"] = GLFW_KEY_CAPS_LOCK;
        input["KEY_SCROLL_LOCK"] = GLFW_KEY_SCROLL_LOCK;
        input["KEY_NUM_LOCK"] = GLFW_KEY_NUM_LOCK;
        input["KEY_PRINT_SCREEN"] = GLFW_KEY_PRINT_SCREEN;
        input["KEY_PAUSE"] = GLFW_KEY_PAUSE;

        // --- Function F1-F25 ---
        input["KEY_F1"] = GLFW_KEY_F1;
        input["KEY_F2"] = GLFW_KEY_F2;
        input["KEY_F3"] = GLFW_KEY_F3;
        input["KEY_F4"] = GLFW_KEY_F4;
        input["KEY_F5"] = GLFW_KEY_F5;
        input["KEY_F6"] = GLFW_KEY_F6;
        input["KEY_F7"] = GLFW_KEY_F7;
        input["KEY_F8"] = GLFW_KEY_F8;
        input["KEY_F9"] = GLFW_KEY_F9;
        input["KEY_F10"] = GLFW_KEY_F10;
        input["KEY_F11"] = GLFW_KEY_F11;
        input["KEY_F12"] = GLFW_KEY_F12;
        input["KEY_F13"] = GLFW_KEY_F13;
        input["KEY_F14"] = GLFW_KEY_F14;
        input["KEY_F15"] = GLFW_KEY_F15;
        input["KEY_F16"] = GLFW_KEY_F16;
        input["KEY_F17"] = GLFW_KEY_F17;
        input["KEY_F18"] = GLFW_KEY_F18;
        input["KEY_F19"] = GLFW_KEY_F19;
        input["KEY_F20"] = GLFW_KEY_F20;
        input["KEY_F21"] = GLFW_KEY_F21;
        input["KEY_F22"] = GLFW_KEY_F22;
        input["KEY_F23"] = GLFW_KEY_F23;
        input["KEY_F24"] = GLFW_KEY_F24;
        input["KEY_F25"] = GLFW_KEY_F25;

        // --- Keypad ---
        input["KEY_KP_0"] = GLFW_KEY_KP_0;
        input["KEY_KP_1"] = GLFW_KEY_KP_1;
        input["KEY_KP_2"] = GLFW_KEY_KP_2;
        input["KEY_KP_3"] = GLFW_KEY_KP_3;
        input["KEY_KP_4"] = GLFW_KEY_KP_4;
        input["KEY_KP_5"] = GLFW_KEY_KP_5;
        input["KEY_KP_6"] = GLFW_KEY_KP_6;
        input["KEY_KP_7"] = GLFW_KEY_KP_7;
        input["KEY_KP_8"] = GLFW_KEY_KP_8;
        input["KEY_KP_9"] = GLFW_KEY_KP_9;
        input["KEY_KP_DECIMAL"] = GLFW_KEY_KP_DECIMAL;
        input["KEY_KP_DIVIDE"] = GLFW_KEY_KP_DIVIDE;
        input["KEY_KP_MULTIPLY"] = GLFW_KEY_KP_MULTIPLY;
        input["KEY_KP_SUBTRACT"] = GLFW_KEY_KP_SUBTRACT;
        input["KEY_KP_ADD"] = GLFW_KEY_KP_ADD;
        input["KEY_KP_ENTER"] = GLFW_KEY_KP_ENTER;
        input["KEY_KP_EQUAL"] = GLFW_KEY_KP_EQUAL;

        // --- Modifiers ---
        input["KEY_LEFT_SHIFT"] = GLFW_KEY_LEFT_SHIFT;
        input["KEY_LEFT_CONTROL"] = GLFW_KEY_LEFT_CONTROL;
        input["KEY_LEFT_ALT"] = GLFW_KEY_LEFT_ALT;
        input["KEY_LEFT_SUPER"] = GLFW_KEY_LEFT_SUPER;

        input["KEY_RIGHT_SHIFT"] = GLFW_KEY_RIGHT_SHIFT;
        input["KEY_RIGHT_CONTROL"] = GLFW_KEY_RIGHT_CONTROL;
        input["KEY_RIGHT_ALT"] = GLFW_KEY_RIGHT_ALT;
        input["KEY_RIGHT_SUPER"] = GLFW_KEY_RIGHT_SUPER;

        input["KEY_MENU"] = GLFW_KEY_MENU;
    }

    void audio_API()
    {
        lua.new_usertype<Sound>("Sound",
            "type", &Sound::type,
            "set_volume", &Sound::setVolume,
            "get_volume", &Sound::getVolume,
            "set_pitch", &Sound::setPitch,
            "get_pitch", &Sound::getPitch,
            "set_looping", &Sound::setLooping,
            "get_looping", &Sound::getLooping
        );
        audio.set_function("play",&Audio::play);
        audio.set_function("pause",&Audio::pause);
        audio.set_function("stop",&Audio::stop);
        audio.set_function("emit_sound",&Audio::emitSound);
    }
    
    void create()
    {
        lua.open_libraries(sol::lib::base, sol::lib::package,sol::lib::string,sol::lib::utf8,sol::lib::table, sol::lib::math);

        luci_API();
        loader_API();
        input_API();
        draw_API();
        audio_API();
        window_API();

        lua.script_file("main.lua");

        load   = lua["_wake"];
        update = lua["_update"];
        draw   = lua["_draw"];
    }
}
