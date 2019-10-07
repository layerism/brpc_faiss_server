//
// Created on 2019-05-21.
//

#ifndef BASE64_H
#define BASE64_H

#include <vector>
#include <string>
#include <iostream>
#include <butil/base64.h>
#include <butil/strings/string_piece.h>
#include "cstring.h"


namespace utils {

static const std::string base64_chars =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789+/";

std::string Base64Encode(unsigned char const *, unsigned int len);
std::string Base64Decode(std::string const &s);

void Base64Encode(std::vector<float> const &vec, std::string &s);
void Base64Decode(std::string const &s, std::vector<float> &vec);
void Base64Decode(butil::StringPiece const &s, std::vector<float> &vec);

}


#endif /* ifndef BASE64_H */
