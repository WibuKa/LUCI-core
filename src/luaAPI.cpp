#include "luaAPI.h"
#include "core/input_system.h"
#include "core/resource.h"
#include "renderer_api.h"
#include "core/window.h"
#include "core/audio.h"
#include "texture.h"

namespace Lua{
    sol::state lua;
    sol::function load;
    sol::function update;
    sol::function draw;

    void luci_API()
    {
        sol::table luci   = lua.create_table();
        sol::table window = lua.create_table();
        luci["window"]    = window;
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
        lua.set_function("load_texture",&Render::load_texture);
        lua.set_function("load_font",&Render::load_font);
        lua.set_function("set_font",&Render::set_font);
        lua.set_function("draw_square",
            [](float x, float y, float w, float h, sol::optional<bool> fill_opt) {
                bool fill = fill_opt.value_or(true);
                Render::drawSquare(x, y, w, h, fill);
            }
        );
        lua.set_function("draw_sprite",
            [](unsigned int id,float x,float y,float scale_x,float scale_y,float angle, sol::optional<int> frame_opt) {
                int frame = frame_opt.value_or(0);
                Render::drawSprite(id,x,y,scale_x,scale_y,angle,frame);
            }
        );

        lua.set_function("draw_text",&Render::drawText);
        lua.set_function("set_color",&Render::setColor);

        lua.new_usertype<Texture>("Texture",
            "width", sol::property(&Texture::get_width),
            "height", sol::property(&Texture::get_height)
        );

        lua.new_usertype<TextureRegion>("TextureRegion",
            sol::constructors<
                TextureRegion(),
                TextureRegion(std::shared_ptr<Texture>),
                TextureRegion(std::shared_ptr<Texture>, int, int, int, int)
            >(),

            "set_texture", &TextureRegion::set_texture,
            "set_region", &TextureRegion::set_region,
            "draw", &TextureRegion::draw
        );            
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
