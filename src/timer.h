#pragma once

namespace Time {
    extern double deltaTime;
    extern double lastFrame;
    extern double targetFrameTime;

    void set_max_framerate(int fps);

}
