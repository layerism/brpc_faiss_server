#ifndef INTERFACES_H_
#define INTERFACES_H_

#include <iostream>
#include <chrono>
#include <ctime>
#include <stdlib.h>
#include "utils/log.h"


namespace interfaces {

void Required(bool &key, std::string);
void Required(int32_t &key, std::string msg);
void Required(int64_t &key, std::string msg);
void Required(std::string &key, std::string msg);


void Optional(bool &key, bool target);
void Optional(int32_t &key, int32_t target);
void Optional(int64_t &key, int64_t target);
void Optional(std::string &key, std::string target);


// context of request
struct Context {
    explicit Context(int session_id){ this->session_id = session_id; }

    void start();
    void finish();
    void success();
    void fail(std::string msg);

    std::string session_id;
    std::string status;
    std::string message;
    std::string start_time;
    std::string end_time;
};

}

#endif // ifndef INTERFACES_H_
