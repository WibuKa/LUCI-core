#include "lua_API.h"
#include "core/input_system.h"
#include "core/resource.h"
#include "renderer.h"
#include "core/window.h"
#include "audio.h"
#include "sol/table.hpp"
#include "texture.h"
#include "texture_region.h"

namespace Lua{
    sol::state lua;
    sol::function load;
    sol::function update;
    sol::function draw;
    
    sol::table luci     = lua.create_table();
    sol::table window   = lua.create_table();
    sol::table texture  = lua.create_table();
    sol::table graphics = lua.create_table();
    sol::table assets   = lua.create_table();

    void luci_API()
    {
        luci["window"]    = window;
        luci["texture"]   = texture;
        luci["assets"]    = assets;
        luci["graphics"]  = graphics;
        lua["luci"]       = luci;

        luci.set_function("clear_color",[](float r,float g, float b){glClearColor(r, g, b, 1.0);});
        window.set_function("set", &Window::set_window);
        window.set_function("resizable", &Window::set_resizable);
        window.set_function("fullscreen", &Window::fullscreen);
    }

    void draw_API()
    {
        lua.set_function("set_view_translate",&Render::setViewTranslate);
        lua.set_function("get_window_size",&Render::getWindowSize);
        lua.set_function("get_texture_size",&Render::getTextureSize);
        lua.set_function("load_texture",&load_texture);
        lua.set_function("load_font",&Render::load_font);
        lua.set_function("set_font",&Render::set_font);

        lua.set_function("draw_text",&Render::drawText);
        lua.set_function("set_color",&Render::setColor);

        lua.new_usertype<Texture>("Texture",
            "width", &Texture::get_width,
            "height",&Texture::get_height
        );

        lua.new_usertype<TextureRegion>("TextureRegion",
            "set_texture", &TextureRegion::set_texture,
            "set_region", &TextureRegion::set_region
        );

        assets.set_function("new_region",&new_texture_region);
        assets.set_function("load_image",&load_texture);
        assets.set_function("load_shader",&Render::loadShader);
        
        graphics.set_function("draw_sprite",&Render::draw_sprite);
        graphics.set_function("draw_rectangle",&Render::draw_rectangle);
        graphics.set_function("draw_circle",&Render::drawCircle);
        graphics.set_function("set_shader",&Render::setShader);
    }

    void input_API()
    {
        lua.set_function("get_mouse_position",&Input::getMousePos);

        sol::table keyboard   = lua.create_table();
        keyboard.set_function("is_down",&Input::isKeyDown);
        keyboard.set_function("is_press",&Input::isKeyPress);
        keyboard.set_function("is_released",&Input::isKeyReleased);
        lua["keyboard"]       = keyboard;

        sol::table mouse_btt  = lua.create_table();
        mouse_btt.set_function("is_down",&Input::isMouseDown);
        mouse_btt.set_function("is_press",&Input::isMousePress);
        mouse_btt.set_function("is_released",&Input::isMouseReleased);
        lua["mouse_button"]    = mouse_btt;
    }

    void audio_API()
    {
        sol::table audio = lua.create_table();
        lua["audio"]     = audio;
        audio.set_function("play",&Audio::play);
        audio.set_function("loop",&Audio::loop);
    }
    
    void create()
    {
        lua.open_libraries(sol::lib::base, sol::lib::package,sol::lib::string,sol::lib::utf8,sol::lib::table, sol::lib::math);

        luci_API();
        draw_API();
        input_API();
        audio_API();


        lua.set_function("preload",&Resource::load);

        lua.script_file("main.lua");

        load   = lua["_wake"];
        update = lua["_update"];
        draw   = lua["_draw"];
    }
}
