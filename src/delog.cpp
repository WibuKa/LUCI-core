#include "delog.h"
#include <cstdio>
#include <cstdarg>

namespace Delog {

    const char* ANSI_RESET = "\033[0m";
    const char* ANSI_GREEN = "\033[32m";
    const char* ANSI_RED   = "\033[31m";
    
    void msg(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        std::vprintf(fmt, args);
        std::printf("\n"); 
        va_end(args);
    }

    void error(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        std::fprintf(stderr, "%s", ANSI_RED);
        std::vfprintf(stderr, fmt, args);
        std::fprintf(stderr, "%s\n", ANSI_RESET);
        va_end(args);
    }
}
