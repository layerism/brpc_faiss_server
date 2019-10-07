/**
 * opencv and brpc test project
 */
#ifndef SERVER_UPDATE_H_
#define SERVER_UPDATE_H_

#include "wrapper_server.h"


class UpdateServer : public base_server::UpdateService {
public:
    UpdateServer(std::shared_ptr<dbase::DataBases> db_ptr){
        this->db_ptr = db_ptr;
    };
    virtual ~UpdateServer(){ };

    DECLARE_SERVER(batch_add_server, batch_add, this->db_ptr);

private:
    virtual void batch_add(
        const base_server::Request *request,
        base_server::Output *output,
        std::shared_ptr<dbase::DataBases> db_ptr
    );

    std::shared_ptr<dbase::DataBases> db_ptr;
};


#endif /* ifndef STATUS_SERVER_H_ */
