/**
 * opencv and brpc test project
 */
#ifndef SERVER_RESTORE_H_
#define SERVER_RESTORE_H_

#include "wrapper_server.h"

class RestoreServer : public base_server::RestoreService {
public:
    RestoreServer(std::shared_ptr<dbase::DataBases> db_ptr){
        this->db_ptr = db_ptr;
    };
    virtual ~RestoreServer(){ };

    DECLARE_SERVER(restore_server, restore, this->db_ptr);

private:

    virtual void restore(
        const base_server::Request *request,
        base_server::Output *output,
        std::shared_ptr<dbase::DataBases> db_ptr
    );

    std::shared_ptr<dbase::DataBases> db_ptr;
};


#endif /* ifndef STATUS_SERVER_H_ */
