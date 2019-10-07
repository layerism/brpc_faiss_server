#include "server_delete.h"


void DeleteServer::delete_(
    const base_server::Request *request,
    base_server::Output *output,
    std::shared_ptr<dbase::DataBases> db_ptr){
    // ===========================
    std::string db_name = request->db_name();
    std::string cid = request->cid();

    interfaces::Required(db_name, "request required db_name !!!");

    if (cid != "") {
        auto &db = db_ptr->at(db_name);
        db.remove(cid);
        output->set_db_name(db.db_name);
        output->set_db_create_time(db.db_create_time);
        output->set_db_size(db.db_size());
        output->set_db_num_of_cluster(db.db_num_of_cluster());
        output->set_feature_dim(db.feature_dim);
    } else {
        db_ptr->purge(db_name);
        output->set_total_database(db_ptr->num_of_dbs());

        for (auto &item:db_ptr->dbs) {
            output->add_db_name_list(item.first);
        }
    }
}
