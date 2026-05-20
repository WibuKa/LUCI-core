#include "timer.h"

namespace Time {
    double deltaTime = 0.0;
    double lastFrame = 0.0;
    double targetFrameTime = 0.0; // 60 FPS

    void set_max_framerate(int fps) {
        targetFrameTime = 1.0 / fps;
    }
}
