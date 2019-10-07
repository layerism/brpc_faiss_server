/**
 * opencv and brpc test project
 */
#ifndef SERVER_CREATE_H_
#define SERVER_CREATE_H_

#include "wrapper_server.h"


class CreateServer : public base_server::CreateService {
public:
    CreateServer(std::shared_ptr<dbase::DataBases> db_ptr){
        this->db_ptr = db_ptr;
    };
    virtual ~CreateServer(){ };

    DECLARE_SERVER(create_server, create, this->db_ptr);

private:

    virtual void create(
        const base_server::Request *request,
        base_server::Output *output,
        std::shared_ptr<dbase::DataBases> db_ptr
    );
    std::shared_ptr<dbase::DataBases> db_ptr;
};


#endif // ifndef SERVER_CREATE_H_
