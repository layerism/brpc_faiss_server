#ifndef MY_STRING_CVT_H_
#define MY_STRING_CVT_H_

#include <iostream>
#include <sstream>
#include <vector>

namespace utils {

typedef unsigned char byte;

typedef union {
    float f;
    unsigned char c[sizeof(float)];
} UnionFloatBytes;

void SplitString(const std::string &s, std::vector<std::string> &v, const std::string &c);

void VectorToStr(const std::vector<float> &vec, std::string &str);
void StrToVector(const std::string &str, std::vector<float> &vec);

void VectorToBytes(const std::vector<float> &vec, std::vector<byte> &byte_array);
void VectorToBytes(const std::vector<float> &vec, std::string &byte_array);
void BytesToVector(const std::vector<byte> &byte_array, std::vector<float> &vec);
void BytesToVector(const std::string &byte_array, std::vector<float> &vec);

}
#endif // ifndef PARSE_H_
