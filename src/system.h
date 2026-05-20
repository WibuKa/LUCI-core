#pragma once
#include <cstdint>

#define WINDOWS_OS 1
#define LINUX_OS 2
#define WEB_BROWSER 3
#define ANDROID_OS 4
#define MAC_OS 5
#define IOS_OS 6
#define UNKNOWN_OS 7

namespace System
{
    extern int operatingSystem;
    extern int cpuCores;
    extern uint64_t ramMB;

    void init();
}
