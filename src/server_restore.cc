#include "server_restore.h"


void RestoreServer::restore(
    const base_server::Request *request,
    base_server::Output *output,
    std::shared_ptr<dbase::DataBases> db_ptr){
    // =========
    std::string db_name = request->db_name();

    auto &db = db_ptr->at(db_name);
    std::string save_path = SAVE_PATH + db_name;
    db.restore(save_path);

    output->set_db_name(db.db_name);
    output->set_db_create_time(db.db_create_time);
    output->set_db_delete_time(db.db_delete_time);
    output->set_db_size(db.db_size());
    output->set_db_num_of_cluster(db.db_num_of_cluster());
    output->set_feature_dim(db.feature_dim);
}
