#include "cstring.h"


namespace utils {

void SplitString(const std::string &s, std::vector<std::string> &v, const std::string &c){
    std::string::size_type pos1, pos2;

    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2) {
        v.push_back(s.substr(pos1, pos2 - pos1));
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length()) {
        v.push_back(s.substr(pos1));
    }
};


void StrToVector(const std::string &str, std::vector<float> &vec){
    std::istringstream in(str);
    std::string t;

    while (in >> t) {
        float val = std::stof(t);
        vec.push_back(val);
    }
}



void VectorToBytes(const std::vector<float> &vec, std::vector<byte> &byte_array){
    byte bytes[sizeof(float)];
    auto it = byte_array.begin();
    for (uint64_t i = 0; i < vec.size(); ++i) {
        *((float *) bytes) = vec[i];
        byte_array.insert(it, &bytes[0], &bytes[sizeof(float)]);
        it = byte_array.end();
    }
}



void VectorToBytes(const std::vector<float> &vec, std::string &byte_array){
    byte bytes[sizeof(float)];
    auto it = byte_array.begin();
    for (uint64_t i = 0; i < vec.size(); ++i) {
        *((float *) bytes) = vec[i];
        byte_array.insert(it, &bytes[0], &bytes[sizeof(float)]);
        it = byte_array.end();
    }
}



void BytesToVector(const std::string &byte_array, std::vector<float> &vec){
    UnionFloatBytes ufb;
    uint64_t idx = 0;
    while (idx < byte_array.size()) {
        for (uint32_t k = 0; k < sizeof(float); ++k) {
            ufb.c[k] = byte_array[idx + k];
        }
        vec.push_back(ufb.f);
        idx += sizeof(float);
    }
}



void BytesToVector(const std::vector<byte> &byte_array, std::vector<float> &vec){
    UnionFloatBytes ufb;
    uint64_t idx = 0;
    while (idx < byte_array.size()) {
        for (uint32_t k = 0; k < sizeof(float); ++k) {
            ufb.c[k] = byte_array[idx + k];
        }
        vec.push_back(ufb.f);
        idx += sizeof(float);
    }
}

}
