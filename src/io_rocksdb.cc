#include "io_rocksdb.h"

namespace dbase {

void RocksDB::open(std::string save_path){
    ThrowErrorIf(this->db_ptr != nullptr, "RocksDB::open() already open a file ...");
    this->options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(this->options, save_path, &this->db_ptr);
    ThrowErrorIf(!status.ok(), "Error in rocksdb::DB::Open ...");
}



void RocksDB::close(){
    delete this->db_ptr;
    this->db_ptr = nullptr;
}



void RocksDB::purge(){
    rocksdb::Iterator *it = this->db_ptr->NewIterator(rocksdb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        this->db_ptr->Delete(rocksdb::WriteOptions(), key);
    }
    delete this->db_ptr;
    this->db_ptr = nullptr;
}



rocksdb::Iterator * RocksDB::iter_key(){
    return this->db_ptr->NewIterator(rocksdb::ReadOptions());
}



void RocksDB::erase(std::string key){
    rocksdb::Status status = this->db_ptr->Delete(rocksdb::WriteOptions(), key);
    ThrowErrorIf(!status.ok(), "Error in RocksDB::delete ...");
}



void RocksDB::get(std::string key, bool &value){
    std::string str_val;
    this->get(key, str_val);
    std::istringstream(str_val) >> value;
}



void RocksDB::get(std::string key, int32_t &value){
    std::string str_val;
    this->get(key, str_val);
    value = std::stol(str_val);
}



void RocksDB::get(std::string key, int64_t &value){
    std::string str_val;
    this->get(key, str_val);
    value = std::stoll(str_val);
}



void RocksDB::get(std::string key, std::vector<float> &value){
    std::string str_val;
    this->get(key, str_val);
    utils::Base64Decode(str_val, value);
}



void RocksDB::get(std::string key, std::string &value){
    ThrowErrorIf(this->db_ptr == nullptr, "RocksDB::get() should open a file first ...");
    rocksdb::Status status = this->db_ptr->Get(rocksdb::ReadOptions(), key, &value);
    ThrowErrorIf(!status.ok(), "Error in db->Get(rocksdb::ReadOptions(), key, &value)");
}



void RocksDB::put(std::string key, std::vector<float> &value){
    std::string str_val;
    utils::Base64Encode(value, str_val);
    this->put(key, str_val);
}



void RocksDB::put(std::string key, bool &value){
    std::string str_val = std::to_string(value);
    this->put(key, str_val);
}



void RocksDB::put(std::string key, int32_t &value){
    std::string str_val = std::to_string(value);
    this->put(key, str_val);
}



void RocksDB::put(std::string key, int64_t &value){
    std::string str_val = std::to_string(value);
    this->put(key, str_val);
}



void RocksDB::put(std::string key, std::string &value){
    ThrowErrorIf(this->db_ptr == nullptr, "RocksDB::put() should open a file first ...");
    size_t length = value.length();
    auto _value = rocksdb::Slice(value.data(), length);
    rocksdb::Status status = this->db_ptr->Put(rocksdb::WriteOptions(), key, _value);
    ThrowErrorIf(!status.ok(), "Error in db->Put(rocksdb::WriteOptions(), key, value)");
}

}
