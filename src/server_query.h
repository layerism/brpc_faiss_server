#ifndef SERVER_QUERY_H_
#define SERVER_QUERY_H_

#include "wrapper_server.h"


class QueryServer : public base_server::QueryService {
public:
    QueryServer(std::shared_ptr<dbase::DataBases> db_ptr){
        this->db_ptr = db_ptr;
    };
    virtual ~QueryServer(){ };

    DECLARE_SERVER(get_status_server, get_status, this->db_ptr);
    DECLARE_SERVER(search_server, search, this->db_ptr);
    DECLARE_ECHO_SERVER(echo_server, echo);

private:
    virtual void get_status(
        const base_server::Request *request,
        base_server::Output *output,
        std::shared_ptr<dbase::DataBases> db_ptr
    );

    virtual void search(
        const base_server::Request *request,
        base_server::Output *output,
        std::shared_ptr<dbase::DataBases> db_ptr
    );

    virtual void echo(
        const base_server::Request *request,
        base_server::Output *output
    );

    std::shared_ptr<dbase::DataBases> db_ptr;
};


#endif /* ifndef STATUS_SERVER_H_ */
