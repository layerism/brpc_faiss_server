/**
 * opencv and brpc test project
 */
#ifndef SERVER_DELETE_H_
#define SERVER_DELETE_H_

#include "wrapper_server.h"


class DeleteServer : public base_server::DeleteService {
public:
    DeleteServer(std::shared_ptr<dbase::DataBases> db_ptr){
        this->db_ptr = db_ptr;
    };
    virtual ~DeleteServer(){ };

    DECLARE_SERVER(delete_server, delete_, this->db_ptr);

private:

    virtual void delete_(
        const base_server::Request *request,
        base_server::Output *output,
        std::shared_ptr<dbase::DataBases> db_ptr
    );

    std::shared_ptr<dbase::DataBases> db_ptr;
};


#endif /* ifndef STATUS_SERVER_H_ */
