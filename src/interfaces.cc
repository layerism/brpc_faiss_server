#include "interfaces.h"


namespace interfaces {

void Required(bool &key, std::string msg){
    ThrowErrorIf(key == 0, msg);
}



void Required(int32_t &key, std::string msg){
    ThrowErrorIf(key == 0, msg);
}



void Required(int64_t &key, std::string msg){
    ThrowErrorIf(key == 0, msg);
}



void Required(std::string &key, std::string msg){
    ThrowErrorIf(key.empty(), msg);
}



void Optional(bool &key, bool target){
    if (key == 0) key = target;
}



void Optional(int32_t &key, int32_t target){
    if (key == 0) key = target;
}



void Optional(int64_t &key, int64_t target){
    if (key == 0) key = target;
}



void Optional(std::string &key, std::string target){
    if (key.empty()) key = target;
}



void Context::start(){
    auto t0 = std::chrono::system_clock::now().time_since_epoch();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(t0);
    double ms = us.count() / 1000.;
    this->start_time = std::to_string(ms);
}



void Context::finish(){
    auto t1 = std::chrono::system_clock::now().time_since_epoch();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1);
    double ms = us.count() / 1000.;
    this->end_time = std::to_string(ms);
}



void Context::success(){
    this->status = "OK";
    this->message = "";
}



void Context::fail(std::string msg){
    this->status = "ERROR";
    this->message = msg;
}

}
