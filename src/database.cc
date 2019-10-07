#include "database.h"

namespace dbase {

void ClusterDict::rehash(int64_t scale){
    this->cfids_to_ids.rehash(scale);
    this->ids_to_cfids.rehash(scale);
    this->clusters.rehash(scale);
}



void ClusterDict::clear(){
    this->clusters.clear();
    this->cfids_to_ids.clear();
    this->ids_to_cfids.clear();
    this->_accumulator = 0;
}



const int64_t &ClusterDict::query(std::string &cid, std::string &fid){
    std::string cfid = cid + DELIMITER + fid;
    return this->cfids_to_ids.at(cfid);
}



bool ClusterDict::has(std::string &cid, std::string &fid){
    std::string cfid = cid + DELIMITER + fid;
    auto it = this->cfids_to_ids.find(cfid);
    if (it != this->cfids_to_ids.end()) {
        return 1;
    } else {
        return 0;
    }
}



bool ClusterDict::has(std::string &cid){
    auto it = this->clusters.find(cid);
    if (it != this->clusters.end()) {
        return 1;
    } else {
        return 0;
    }
}



void ClusterDict::erase(std::string &cid, std::string &fid){
    std::string cfid = cid + DELIMITER + fid;
    int64_t id = this->cfids_to_ids.at(cfid);
    this->ids_to_cfids.erase(id);
    this->cfids_to_ids.erase(cfid);

    std::vector<std::string> &fids = this->clusters.at(cid);
    auto it = std::find(fids.begin(), fids.end(), fid);
    if (it != fids.end())
        fids.erase(it);

    // if cid become empty, delete this cid
    if (fids.size() == 0)
        this->clusters.erase(cid);
}



void ClusterDict::add(std::string &cid, std::string &fid){
    std::string cfid = cid + DELIMITER + fid;
    int64_t id = this->_accumulator;
    this->cfids_to_ids.insert(std::make_pair(cfid, id));
    this->ids_to_cfids.insert(std::make_pair(id, cfid));

    if (this->clusters.find(cid) == this->clusters.end()) {
        std::vector<std::string> fid_list;
        this->clusters.insert(std::make_pair(cid, fid_list));
    }

    this->clusters.at(cid).push_back(fid);
    this->_accumulator += 1;
}



void ClusterDict::pprint(std::string &cid){
    std::string result = "cid: " + cid + ", fids=[";
    auto it = this->clusters.find(cid);
    if (it == this->clusters.end()) {
        LOG(INFO) << "no such user: " + cid;
    } else {
        const std::vector<std::string> &fids = this->clusters.at(cid);
        for (uint64_t i = 0; i < fids.size(); ++i) {
            result = result + fids[i];
            if (i != fids.size() - 1)
                result += ", ";
        }
        result += "]";
        LOG(INFO) << result;
    }
}



void DataBase::build_faiss_index(){
    if (this->search_type == 0 && this->similarity_type == "L2") {
        faiss::IndexFlatL2 *ptr_faiss_index = new faiss::IndexFlatL2(this->feature_dim);
        this->faiss_index = new faiss::IndexIDMap(ptr_faiss_index);
        this->is_l2_norm = 0;
        this->clusters.rehash(MEDIUM_SCALE);
        LOG(INFO) << "Init faiss::IndexFlatL2 sucess !!";
    } else if (this->search_type == 0 && this->similarity_type == "cosine") {
        faiss::IndexFlatIP *ptr_faiss_index = new faiss::IndexFlatIP(this->feature_dim);
        this->faiss_index = new faiss::IndexIDMap(ptr_faiss_index);
        this->is_l2_norm = 1;
        this->clusters.rehash(MEDIUM_SCALE);
        LOG(INFO) << "Init faiss::IndexFlatIP sucess !!";
    } else if (this->search_type == 0 && this->similarity_type == "IP") {
        faiss::IndexFlatIP *ptr_faiss_index = new faiss::IndexFlatIP(this->feature_dim);
        this->faiss_index = new faiss::IndexIDMap(ptr_faiss_index);
        this->is_l2_norm = 0;
        this->clusters.rehash(MEDIUM_SCALE);
        LOG(INFO) << "Init faiss::IndexFlatIP cosine sucess !!";
    }
}



void DataBase::create(
    std::string db_name,
    int32_t feature_dim,
    std::string search_device = "cpu",
    std::string feature_version = "v1",
    int32_t search_type = 0,
    std::string similarity_type = "L2"){
    // =====================
    std::lock_guard<std::mutex> lock(this->g_mutex);
    this->db_name = db_name;
    this->feature_dim = feature_dim;
    this->db_create_time = utils::localTime();
    this->db_delete_time = "";
    this->search_device = search_device;
    this->feature_version = feature_version;
    this->search_type = search_type;
    this->similarity_type = similarity_type;

    this->clusters.init();
    this->save_db.open(SAVE_PATH + this->db_name);
    this->save_db.put("meta:db_name", this->db_name);
    this->save_db.put("meta:feature_dim", this->feature_dim);
    this->save_db.put("meta:search_device", this->search_device);
    this->save_db.put("meta:feature_version", this->feature_version);
    this->save_db.put("meta:search_type", this->search_type);
    this->save_db.put("meta:similarity_type", this->similarity_type);

    if (this->is_build == 0) {
        this->build_faiss_index();
        this->is_build = 1;
    } else {
        ThrowError("db_name: " + this->db_name + " have been created");
    }
}



void DataBase::clear(){
    std::lock_guard<std::mutex> lock(this->g_mutex);
    if (this->is_build == 1) {
        delete this->faiss_index;
        this->faiss_index = nullptr;
        this->db_name = "";
        this->feature_dim = 0;
        this->db_delete_time = utils::localTime();
        this->clusters.clear();
        this->save_db.purge();
        this->is_build = 0;
        LOG(INFO) << "success clear the data base !!!";
    } else {
        ThrowError("db_name: " + this->db_name + " do not exist");
    }
}



void DataBase::remove(std::string &cid){
    std::lock_guard<std::mutex> lock(this->g_mutex);
    if (this->clusters.has(cid)) {
        std::vector<std::string> fids = this->clusters.query(cid);
        for (uint64_t i = 0; i < fids.size(); ++i) {
            // this->remove(cid, fids[i]);
            int64_t id = this->clusters.query(cid, fids[i]);
            this->clusters.erase(cid, fids[i]);
            faiss::IDSelectorRange del_id(id, id + 1);
            this->faiss_index->remove_ids(del_id);
            std::string key = cid + DELIMITER + fids[i];
            this->save_db.erase(key);
        }
    } else {
        ThrowError("remove fail ! no such cid in db !");
    }
}



void DataBase::remove(std::string &cid, std::string &fid){
    std::lock_guard<std::mutex> lock(this->g_mutex);
    if (this->clusters.has(cid, fid)) {
        int64_t id = this->clusters.query(cid, fid);
        this->clusters.erase(cid, fid);
        faiss::IDSelectorRange del_id(id, id + 1);
        this->faiss_index->remove_ids(del_id);
        std::string key = cid + DELIMITER + fid;
        this->save_db.erase(key);
    } else {
        ThrowError("remove fail ! no (cid, fid) in db !");
    }
}



void DataBase::add(std::string &cid, std::string &fid, std::string &b64_feature){
    std::lock_guard<std::mutex> lock(this->g_mutex);
    ThrowErrorIf(this->is_build == 0, "should create a data base first");

    std::vector<float> feature;
    utils::Base64Decode(b64_feature, feature);

    // this->clusters.add(cid, fid);
    // int64_t id = this->clusters.query(cid, fid);
    // this->faiss_index->add_with_ids(1, &feature[0], &id);

    if (!this->clusters.has(cid, fid)) {
        // cid and fid no exist, add a new one
        this->clusters.add(cid, fid);
        int64_t id = this->clusters.query(cid, fid);
        this->faiss_index->add_with_ids(1, &feature[0], &id);
    } else {
        // if cid and fid exist, overwrite the feature
        const int64_t &id = this->clusters.query(cid, fid);
        faiss::IDSelectorRange del_id(id, id + 1);
        this->faiss_index->remove_ids(del_id);
        this->faiss_index->add_with_ids(1, &feature[0], &id);
    }
}



void DataBase::add_and_save(std::string &cid, std::string &fid, std::string &b64_feature){
    this->add(cid, fid, b64_feature);
    std::string key = cid + DELIMITER + fid;
    this->save_db.put(key, b64_feature);
}



std::vector<Recall> DataBase::search(std::string &b64_feature, int32_t topk){
    ThrowErrorIf(this->is_build == 0, "should create a data base first");
    ThrowErrorIf(this->db_size() == 0, "no feature in database, should add feature");

    std::vector<float> feature;
    utils::Base64Decode(b64_feature, feature);

    int64_t *recall_idx = new int64_t[topk];
    float *recall_distance = new float[topk];

    this->faiss_index->search(1, &feature[0], topk, recall_distance, recall_idx);

    std::vector<Recall> recalls;
    for (int i = i; i < topk; ++i) {
        Recall recall;
        int64_t id = recall_idx[i];
        const std::string &cfid = this->clusters.query(id);
        recall.cfid = cfid;
        recall.similarity = recall_distance[i];
        recalls.push_back(recall);
    }

    delete recall_idx;
    delete recall_distance;
    return recalls;
}



void DataBase::restore(std::string &db_name){
    // ===============
    std::lock_guard<std::mutex> lock(this->g_mutex);
    std::string save_path = SAVE_PATH + db_name;
    this->save_db.open(save_path);
    this->save_db.get("meta:db_name", this->db_name);
    this->save_db.get("meta:feature_dim", this->feature_dim);
    this->save_db.get("meta:search_device", this->search_device);
    this->save_db.get("meta:feature_version", this->feature_version);
    this->save_db.get("meta:search_type", this->search_type);
    this->save_db.get("meta:similarity_type", this->similarity_type);

    auto it = this->save_db.iter_key();
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        // only process non meta key
        if (key.find("meta:") == std::string::npos) {
            std::string b64_feature;
            this->save_db.get(key, b64_feature);
            std::vector<std::string> cfid;
            utils::SplitString(key, cfid, DELIMITER);
            std::string cid = cfid[0];
            std::string fid = cfid[1];
            this->add(cid, fid, b64_feature);
        }
    }
}



DataBase &DataBases::at(std::string &db_name){
    if (this->has(db_name)) {
        auto *db = this->dbs.at(db_name);
        return *db;
    } else {
        ThrowError("db is no exist: " + db_name);
    }
}



bool DataBases::has(std::string &db_name){
    auto it = this->dbs.find(db_name);
    if (it != this->dbs.end())
        return 1;
    else
        return 0;
}



void DataBases::create(
    std::string db_name,
    int32_t feature_dim,
    std::string search_device = "cpu",
    std::string feature_version = "v1",
    int32_t search_type = 0,
    std::string similarity_type = "L2"){
    ///=====================
    if (!this->has(db_name)) {
        DataBase *db = new DataBase;
        db->create(
            db_name,
            feature_dim,
            search_device,
            feature_version,
            search_type,
            similarity_type
        );
        this->dbs.insert(std::make_pair(db_name, db));
    } else {
        ThrowError("db is exist: " + db_name);
    }
}



void DataBases::purge(std::string &db_name){
    if (this->has(db_name)) {
        auto &db = this->dbs.at(db_name);
        db->clear();
        delete db;
        db = nullptr;
        this->dbs.erase(db_name);
    } else {
        ThrowError("db is no exist: " + db_name);
    }
}

}
