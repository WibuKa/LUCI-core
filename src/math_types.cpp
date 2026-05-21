#include "math_types.h"

namespace Math_ {
    int nextPowerOfTwo(int v)
    {
        int r = 1;
        while (r < v) r <<= 1;
        return r;
    }
}
