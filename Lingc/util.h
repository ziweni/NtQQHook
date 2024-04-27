#ifndef UTIL_H
#define UTIL_H
#include <curl/curl.h>
#include <string>
#include <vector>

class util
{
public:
	static size_t hex2bin(uint8_t* dest, const char* src, size_t srclen);
	static std::string bin2hex(uint8_t* in, size_t len);
	static bool readFile(std::string path, std::vector<unsigned char>& buffer);
};
#endif