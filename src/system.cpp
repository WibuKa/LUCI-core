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
        
        #if defined(_WIN32)
            operatingSystem = WINDOWS_OS;

        #elif defined(__linux__)
            operatingSystem = LINUX_OS;
        #else
            operatingSystem = UNKNOWN_OS;
        #endif

        // Get RAM size
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

        // Get CPU cores
        cpuCores = static_cast<int>(std::thread::hardware_concurrency());
        
        // Enable ANSI escape codes for colored terminal output for windows
        #if defined(_WIN32)
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD dwMode = 0;
            GetConsoleMode(hOut, &dwMode);
            SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        #endif
    }
}
