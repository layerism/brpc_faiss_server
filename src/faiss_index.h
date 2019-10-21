#ifndef _FAISS_INDEX_H
#define _FAISS_INDEX_H

#include <algorithm>
#include <stdio.h>
#include <cstdint>

#include "faiss/IndexFlat.h"
#include "faiss/IndexIVFFlat.h"
#include "faiss/IndexIVFPQ.h"
#include "faiss/AuxIndexStructures.h"
#include "faiss/MetaIndexes.h"

#include "utils/log.h"
#include "utils/cstring.h"
#include "tensor/linalg.h"
#include "config.h"

namespace dbase {

typedef int64_t idx_t;

/*
 * a wrapper for faiss index
 */
class FaissIndex {
public:
    void init(uint32_t dim, std::string metric);
    void search(std::vector<float> &x, int32_t topk, float *distance, int64_t *idx);
    void erase(const int64_t &id);
    void add_with_id(std::vector<float> &x, const int64_t &id);
    void update_with_id(std::vector<float> &x, const int64_t &id);
    void purge();

private:
    faiss::IndexFlat *_flat_index = nullptr;
    faiss::IndexFlat *_quantizer = nullptr;
    faiss::IndexIVFFlat *_index = nullptr;

    faiss::IndexIDMap *index = nullptr;
    faiss::IndexIDMap *flat_index = nullptr;

    uint32_t n_centroids = NLIST;
    uint32_t nprobe = NPROBE;
    uint32_t init_n_centroids = 0;
    uint32_t max_n = 0;
    float alpha = ALPHA;

    uint32_t feature_dim = 0;
    faiss::MetricType metric_type;
    std::string index_type;
    std::string device;

    std::vector<uint32_t> centroids_bucket_size;
    std::vector<float> x_buff;
    std::vector<int64_t> id_buff;
    bool is_train_finish = false;
    bool required_l2_norm = true;

    void build_index();
    void online_train_quantizer(const std::vector<float> &x, const int64_t &id);
    void shrink_quantizer();
};

}

#endif /* ifndef DATA_BASE_H */
