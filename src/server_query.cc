#include "server_query.h"


void QueryServer::get_status(
    const base_server::Request *request,
    base_server::Output *output,
    std::shared_ptr<dbase::DataBases> db_ptr){
    // ============================
    std::string db_name = request->db_name();
    std::string cid = request->cid();

    if (db_name != "") {
        auto &db = db_ptr->at(db_name);

        if (cid != "") {
            output->set_db_name(db.db_name);
            output->set_feature_dim(db.feature_dim);
            output->set_feature_version(db.feature_version);
            output->set_db_cluster_size(db.db_cluster_size(cid));
        } else {
            output->set_db_name(db.db_name);
            output->set_db_create_time(db.db_create_time);
            output->set_db_delete_time(db.db_delete_time);
            output->set_feature_dim(db.feature_dim);
            output->set_feature_version(db.feature_version);
            output->set_search_type(db.search_type);
            output->set_similarity_type(db.similarity_type);
            output->set_db_size(db.db_size());
            output->set_db_num_of_cluster(db.db_num_of_cluster());
        }
    } else {
        output->set_total_database(db_ptr->num_of_dbs());
        for (auto &item:db_ptr->dbs) {
            output->add_db_name_list(item.first);
        }
    }
}



void QueryServer::search(
    const base_server::Request *request,
    base_server::Output *output,
    std::shared_ptr<dbase::DataBases> db_ptr){
    // ===========================
    std::string db_name = request->db_name();
    std::string b64_feature = request->b64_feature();
    int32_t topk = request->topk();

    interfaces::Required(db_name, "request required db_name !!!");
    interfaces::Required(b64_feature, "request required b64_feature !!!");
    interfaces::Required(topk, "request required topk !!!");

    auto &db = db_ptr->at(db_name);
    auto recalls = db.search(b64_feature, topk);

    for (auto &recall:recalls) {
        auto r = output->add_recall();
        r->set_cfid(recall.cfid);
        r->set_similarity(recall.similarity);
    }
}



void QueryServer::echo(
    const base_server::Request *request,
    base_server::Output *output){
    // ===========================
    std::string db_name = request->db_name();
    output->set_db_name(db_name);
}
