#ifndef IO_ROCKSDB_H
#define IO_ROCKSDB_H

#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include <cstdint>
#include <thread>
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "utils/cstring.h"
#include "utils/log.h"
#include "utils/base64.h"


namespace dbase {

struct RocksDB {
    rocksdb::DB *db_ptr = nullptr;
    rocksdb::Options options;

    void open(std::string save_path);

    void put(std::string key, bool &value);
    void put(std::string key, int32_t &value);
    void put(std::string key, int64_t &value);
    void put(std::string key, std::string &value);
    void put(std::string key, std::vector<float> &value);
    // void put(std::string key, const char *value, size_t length);

    void get(std::string key, bool &value);
    void get(std::string key, int32_t &value);
    void get(std::string key, int64_t &value);
    void get(std::string key, std::string &value);
    void get(std::string key, std::vector<float> &value);

    rocksdb::Iterator * iter_key();
    void erase(std::string key);
    void close();
    void purge();
};

}
#endif /* ifndef IO_ROCKSDB_H */
