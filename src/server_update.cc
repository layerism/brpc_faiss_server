#include "server_update.h"


void UpdateServer::batch_add(
    const base_server::Request *request,
    base_server::Output *output,
    std::shared_ptr<dbase::DataBases> db_ptr){
    // =========
    std::string db_name = request->db_name();
    std::int32_t feature_dim = request->feature_dim();
    std::string feature_version = request->feature_version();

    interfaces::Required(db_name, "request required db_name !!!");
    interfaces::Required(feature_dim, "request required feature_dim !!!");
    interfaces::Optional(feature_version, "v0");

    auto &db = db_ptr->at(db_name);

    for (int i = 0; i < request->data_size(); ++i) {
        auto data_i = request->data(i);
        std::string cid = data_i.cid();
        std::string fid = data_i.feature_id();
        std::string b64_feature = data_i.b64_feature();
        interfaces::Required(cid, "request required data.cid !!!");
        interfaces::Required(fid, "request required data.feature_id !!!");
        interfaces::Required(b64_feature, "request required data.b64_feature !!!");
        db.add_and_save(cid, fid, b64_feature);
    }

    output->set_db_name(db.db_name);
    output->set_db_create_time(db.db_create_time);
    output->set_db_delete_time(db.db_delete_time);
    output->set_db_size(db.db_size());
    output->set_db_num_of_cluster(db.db_num_of_cluster());
    output->set_feature_dim(db.feature_dim);
}
