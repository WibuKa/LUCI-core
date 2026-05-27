#pragma once
#include <vector>
#include <string>
#include "primitive.h"

struct Mesh {
    std::string name;
    std::vector<Primitive> primitives;
    
    int SkinIndex = -1;
};
