#ifndef LOG_H_
#define LOG_H_

#include <butil/logging.h>
#include <butil/strings/string_piece.h>
#include <random>
#include "cstring.h"

#define ThrowError(msg)         throw std::runtime_error(msg)
#define ThrowErrorIf(cond, msg) if (cond) throw std::runtime_error (msg)
#define VarName(x)              #x
#define IsSameType(t, x)        std::is_same<t, x>::value


namespace utils {

unsigned int random_char();
std::string generate_uuid(unsigned int len);
std::string localTime();

template<typename T>
void pprint(const std::vector<T> &vec){
    std::cout << std::endl;
    for (uint32_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << ", ";
    }
    std::cout << std::endl;
}

}


#endif // ifndef LOG_H_
