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
void pprint(const std::vector<float> &vec);
std::string localTime();

}


#endif // ifndef LOG_H_
