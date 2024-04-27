#include "util.h"
#include <fstream>
#include <string>
#include <vector>

bool util::readFile(std::string path, std::vector<unsigned char>& buffer) {
    std::ifstream file(path, std::ios::binary);
    if (file.fail()) {
        perror(path.c_str());
        return false;
    }
    // 把文件读取指针移动到末尾并获取位置
    file.seekg(0, std::ios::end);
    std::streamoff fileSize = file.tellg();
    // 把文件读取指针移动到首字节位置并与之前的相减
    file.seekg(0, std::ios::beg);
    fileSize -= file.tellg();
    buffer.resize(fileSize);
    file.read((char*)&(buffer[0]), fileSize);
    file.close();
    return true;
}

void cbin2hex(char* out, uint8_t* in, size_t len)
{
    if (out) {
        unsigned int i;
        for (i = 0; i < len; i++)
            sprintf(out + (i * 2), "%02x", (uint8_t)in[i]);
    }
}

std::string util::bin2hex(uint8_t* in, size_t len)
{
    char* s = new char[(len * 2) + 1]();
    if (!s)
        return NULL;
    cbin2hex(s, in, len);
    return std::string(s);
}

size_t util::hex2bin(uint8_t* dest, const char* src, size_t srclen) {
    size_t i = 0;
    if (srclen % 2 != 0) return 0;
    for (i = 0; i < srclen / 2; i++)
    {
        char tmp[3];
        tmp[0] = *(src + 2 * i);
        tmp[1] = *(src + 2 * i + 1);
        tmp[2] = 0;
        int out = 0;
        sscanf(tmp, "%x", &out);
        uint8_t ch = (uint8_t)out;
        *(dest + i) = ch;
    }
    return i;
}
