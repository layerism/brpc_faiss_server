#include "log.h"


namespace utils {

unsigned int random_char(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    return dis(gen);
}



std::string generate_uuid(unsigned int len){
    std::stringstream ss;
    for (unsigned int i = 0; i < len; i++) {
        const auto rc = random_char();
        std::stringstream hexstream;
        hexstream << std::hex << rc;
        auto hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
}



void pprint(const std::vector<float> &vec){
    std::string output;
    VectorToStr(vec, output);
    for (uint64_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << ", ";
    }
    std::cout << std::endl;
}



std::string localTime(){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::string year = std::to_string(1900 + ltm->tm_year);
    std::string month = std::to_string(1 + ltm->tm_mon);
    std::string day = std::to_string(ltm->tm_mday);
    std::string hour;
    std::string min;
    std::string sec;
    if (ltm->tm_hour + 8 < 10)      // GMT to local hour, +8 hour
        hour = "0" + std::to_string(ltm->tm_hour + 8);
    else
        hour = std::to_string(ltm->tm_hour + 8);

    if (ltm->tm_min < 10)
        min = "0" + std::to_string(ltm->tm_min);
    else
        min = std::to_string(ltm->tm_min);

    if (ltm->tm_sec < 10)
        sec = "0" + std::to_string(ltm->tm_sec);
    else
        sec = std::to_string(ltm->tm_sec);

    std::string date;
    date = year + "-" + month + "-" + day + " " + hour + ":" + min + ":" + sec;
    return date;
}

}
