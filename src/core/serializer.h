#pragma once
#include <string>
#include <vector>
#include "object.h"

namespace Serializer{

ObjectData load_ref(std::string &path);
ObjectData load_file_ref(std::string &path);

void load_scene(std::string path);

}