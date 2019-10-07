#include "base64.h"


namespace utils {

static inline bool is_base64(unsigned char c){
    return (isalnum(c) || (c == '+') || (c == '/'));
}



std::string Base64Encode(unsigned char const *bytes_to_encode, unsigned int in_len){
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';
    }

    return ret;
}



std::string Base64Decode(std::string const &encoded_string){
    size_t in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]) & 0xff;

            char_array_3[0] = ( char_array_4[0] << 2       ) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = 0; j < i; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]) & 0xff;

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}



void Base64Encode(std::vector<float> const &vec, std::string &s){
    std::string byte_array;
    byte bytes[sizeof(float)];
    auto it = byte_array.begin();
    for (uint64_t i = 0; i < vec.size(); ++i) {
        *((float *) bytes) = vec[i];
        byte_array.insert(it, &bytes[0], &bytes[sizeof(float)]);
        it = byte_array.end();
    }
    butil::StringPiece input = byte_array;
    butil::Base64Encode(input, &s);
}



void Base64Decode(std::string const &s, std::vector<float> &vec){
    butil::StringPiece str = s;
    std::string byte_array;
    butil::Base64Decode(str, &byte_array);

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



void Base64Decode(butil::StringPiece const &s, std::vector<float> &vec){
    std::string byte_array;
    butil::Base64Decode(s, &byte_array);

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
