#include "system.h"
#include <thread>

#if defined(_WIN32)
    #define NOMINMAX
    #include <windows.h>
#elif defined(__linux__)
    #include <sys/sysinfo.h>
#endif

int System::operatingSystem = UNKNOWN_OS;
int System::cpuCores = 1;
uint64_t System::ramMB = 0;

namespace System
{
    void init()
    {
        cpuCores = static_cast<int>(std::thread::hardware_concurrency());
        
        #if defined(_WIN32)
            operatingSystem = WINDOWS_OS;
        #elif defined(__linux__)
            operatingSystem = LINUX_OS;
        #else
            operatingSystem = UNKNOWN_OS;
        #endif


        #if defined(_WIN32)
            MEMORYSTATUSEX status;
            status.dwLength = sizeof(status);
            GlobalMemoryStatusEx(&status);
            ramMB = status.ullTotalPhys / (1024ull * 1024ull);
        #elif defined(__linux__)
            struct sysinfo info;
            sysinfo(&info);
            ramMB =(info.totalram * info.mem_unit)/ (1024ull * 1024ull);
        #else
            ramMB = 0;
        #endif
    }
}
