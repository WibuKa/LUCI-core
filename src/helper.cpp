#include "helper.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>

#include <string>
#include <vector>

std::vector<uint32_t> string2U32(const std::string& s) {
    std::vector<uint32_t> out;
    uint32_t code = 0;
    int bytes = 0;

    for (unsigned char c : s) {
        if (c <= 0x7F) {
            // ASCII
            out.push_back(c);
        }
        else if ((c >> 6) == 0b10) {
            // Tiếp tục byte UTF-8
            code = (code << 6) | (c & 0x3F);
            if (--bytes == 0) {
                out.push_back(code);
            }
        }
        else {
            // Bắt đầu một ký tự UTF-8 mới
            if ((c >> 5) == 0b110) {
                bytes = 1;
                code = c & 0x1F;
            }
            else if ((c >> 4) == 0b1110) {
                bytes = 2;
                code = c & 0x0F;
            }
            else if ((c >> 3) == 0b11110) {
                bytes = 3;
                code = c & 0x07;
            }
        }
    }
    return out;
}
