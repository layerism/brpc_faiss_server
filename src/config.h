#ifndef CONFIG_H
#define CONFIG_H


// pre define the scale of the data set,
#define SMALL_SCALE  1000000
#define MEDIUM_SCALE 10000000
#define LARGE_SCALE  100000000
#define SUPER_SCALE  1000000000
#define EARTH_SCALE  10000000000


#define SAVE_PATH        "/data/saved_rocksdb_faiss/"
#define DELIMITER        "_"
#define IP_PORT          8330
#define IDLE_TIMEOUT_SEC 2000
#define MAX_RETRY        3
#define URL              "/brpc_faiss_server"

#define DELETE_APP_ROUTE \
    URL "/delete => delete_server"

#define CREATE_APP_ROUTE \
    URL "/create => create_server,"

#define QUERY_APP_ROUTE \
    URL "/status => get_status_server," \
    URL "/search => search_server," \
    URL "/echo => echo_server"

#define RESTORE_APP_ROUTE \
    URL "/restore => restore_server"

#define UPDATE_APP_ROUTE \
    URL "/batch_add => batch_add_server"


#endif /* ifndef CONFIG_H */
