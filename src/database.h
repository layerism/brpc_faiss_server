#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <algorithm>
#include <stdio.h>
#include <cstdint>
#include <mutex>

#include "faiss/IndexFlat.h"
#include "faiss/IndexIVFFlat.h"
#include "faiss/IndexIVFPQ.h"
#include "faiss/AuxIndexStructures.h"
#include "faiss/MetaIndexes.h"

#include "utils/cstring.h"
#include "utils/log.h"
#include "utils/base64.h"
#include "io_rocksdb.h"
#include "config.h"


namespace dbase {

struct Recall {
    std::string cfid;
    float similarity;
};


struct ClusterDict {
    /*
     * maintaining the following dict:
     * - clusters: Map[cid] = [fid_1, fid_2, fid_3, ...]
     * - cfids_to_ids: Map[cfid = id
     * - ids_to_cfids: Map[id] = cfid
     *
     * in which:
     * 1. cid: cluster id, tell which cluster the feature belong to
     * 2. fid: feature id, a unique id for the vector feature
     * 3. id:  id for faiss index, because faiss require continue memory search
     * 4. cfid: cid + "_" + fid
     */

    std::unordered_map<std::string, std::vector<std::string>> clusters;
    std::unordered_map<std::string, int64_t> cfids_to_ids;
    std::unordered_map<int64_t, std::string> ids_to_cfids;

    void init(){ this->_accumulator = 0; }

    void rehash(int64_t scale);
    void clear();
    void erase(std::string &cid, std::string &fid);
    bool has(std::string &cid);
    bool has(std::string &cid, std::string &fid);
    void add(std::string &cid, std::string &fid);
    const std::vector<std::string> &query(std::string &cid){ return this->clusters.at(cid); }

    const std::string &query(int64_t &id){ return this->ids_to_cfids.at(id); }

    const int64_t &query(std::string &cid, std::string &fid);
    int64_t size(std::string &cid){ return this->clusters.at(cid).size(); }

    int64_t size(){ return this->clusters.size(); }

    int64_t total_size(){ return this->cfids_to_ids.size(); }

    void pprint(std::string &cid);

    int64_t _accumulator = 0;
};


class DataBase {
public:
    // unique name for the data base
    std::string db_name = "";
    int32_t db_version = 0;

    std::string db_create_time = "";
    std::string db_update_time = "";
    std::string db_delete_time = "";

    std::string search_device = "cpu";
    std::string feature_version = "";

    // 0: SMALL_SCALE, 1: MEDIUM_SCALE,
    // 2: LARGE_SCALE, 3: SUPER_SCALE, 4: EARTH_SCALE
    int32_t search_type = 0;

    // "L2": |x-y|^2, "cosine": 2 - 2 * dot(x,y)/|x||y|, "IP": dot(x,y)
    std::string similarity_type = "L2";

    int32_t feature_dim = 0;

    void create(
        std::string db_name,
        int32_t feature_dim,
        std::string search_device,
        std::string feature_version,
        int32_t search_type,
        std::string similarity_type
    );

    void clear();
    void add(std::string &cid, std::string &fid, std::string &b64_feature);
    void add_and_save(std::string &cid, std::string &fid, std::string &b64_feature);
    void add(std::string &cid, std::string &fid, std::vector<float> &feature);
    void remove(std::string &cid, std::string &fid);
    void remove(std::string &cid);
    std::vector<Recall> search(std::string &b64_feature, int32_t topk);
    void restore(std::string &save_path);
    int64_t db_size(){ return this->clusters.total_size(); }

    int64_t db_cluster_size(std::string &cid){ return this->clusters.size(cid); }

    int64_t db_num_of_cluster(){ return this->clusters.size(); }

    void pprint(std::string &cid){ this->clusters.pprint(cid); }

private:
    ClusterDict clusters;
    faiss::IndexIDMap *faiss_index = nullptr;
    RocksDB save_db;
    bool is_build = 0;
    bool is_l2_norm = 0;
    std::mutex g_mutex;

    void build_faiss_index();
};


class DataBases {
public:
    std::unordered_map<std::string, DataBase *> dbs;

    DataBase &at(std::string &db_name);
    bool has(std::string &db_name);
    void create(
        std::string db_name,
        int32_t feature_dim,
        std::string search_device,
        std::string feature_version,
        int32_t search_type,
        std::string similarity_type
    );
    void purge(std::string &db_name);

    int32_t num_of_dbs(){ return this->dbs.size(); }
};

}


#endif /* ifndef DATA_BASE_H */
