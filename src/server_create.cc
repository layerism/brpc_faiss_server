#include "server_create.h"


void CreateServer::create(
    const base_server::Request *request,
    base_server::Output *output,
    std::shared_ptr<dbase::DataBases> db_ptr){
    // ==========================

    std::string db_name = request->db_name();
    int32_t feature_dim = request->feature_dim();
    std::string search_device = request->search_device();
    std::string feature_version = request->feature_version();
    int32_t search_type = request->search_type();
    std::string similarity_type = request->similarity_type();

    interfaces::Required(db_name, "request required db_name !!!");
    interfaces::Required(feature_dim, "request required feature_dim !!!");
    interfaces::Optional(search_device, "cpu");
    interfaces::Optional(feature_version, "v1");
    interfaces::Optional(search_type, 0);
    interfaces::Optional(similarity_type, "L2");

    db_ptr->create(
        db_name,
        feature_dim,
        search_device,
        feature_version,
        search_type,
        similarity_type
    );

    auto &db = db_ptr->at(db_name);
    output->set_total_database(db_ptr->num_of_dbs());
    output->set_db_name(db.db_name);
    output->set_db_size(db.db_size());
    output->set_db_num_of_cluster(db.db_num_of_cluster());
    output->set_db_create_time(db.db_create_time);
}
