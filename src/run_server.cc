#include <gflags/gflags.h>
#include <brpc/server.h>
#include "config.h"
#include "server_query.h"
#include "server_create.h"
#include "server_update.h"
#include "server_delete.h"
#include "server_restore.h"


void AddService(
    brpc::Server &server,
    google::protobuf::Service *impl,
    const butil::StringPiece &restful_mappings){
    // ==================
    if (server.AddService(impl, brpc::SERVER_DOESNT_OWN_SERVICE, restful_mappings) != 0) {
        std::string mappings = restful_mappings.as_string();
        LOG(ERROR) << "fail to add service: " + mappings;
        throw std::runtime_error("fail to add service:" + mappings);
    }
}



void StartService(brpc::Server &server){
    // Start the server.
    brpc::ServerOptions options;
    int port = IP_PORT;
    options.idle_timeout_sec = IDLE_TIMEOUT_SEC;
    if (server.Start(port, &options) != 0) {
        LOG(ERROR) << "fail to start EchoServer";
        throw std::runtime_error("fail to start EchoServer");
    }
}



int main(int argc, char *argv[]){
    // Generally you only need one Server.

    std::shared_ptr<dbase::DataBases> db_ptr(new dbase::DataBases);

    brpc::Server server;

    QueryServer wrapper_query_impl(db_ptr);
    AddService(server, &wrapper_query_impl, QUERY_APP_ROUTE);

    CreateServer wrapper_create_impl(db_ptr);
    AddService(server, &wrapper_create_impl, CREATE_APP_ROUTE);

    UpdateServer wrapper_update_impl(db_ptr);
    AddService(server, &wrapper_update_impl, UPDATE_APP_ROUTE);

    DeleteServer wrapper_delete_impl(db_ptr);
    AddService(server, &wrapper_delete_impl, DELETE_APP_ROUTE);

    RestoreServer wrapper_restore_impl(db_ptr);
    AddService(server, &wrapper_restore_impl, RESTORE_APP_ROUTE);

    StartService(server);
    server.RunUntilAskedToQuit();

    return 0;
}
