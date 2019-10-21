#ifndef CONFIG_H
#define CONFIG_H


// pre define the scale of the data set,
#define LEVEL_1_SCALE          100000
#define LEVEL_2_SCALE          1000000
#define LEVEL_3_SCALE          10000000
#define LEVEL_4_SCALE          100000000
#define LEVEL_5_SCALE          1000000000

#define LEVEL_1_NLIST          256 // 0.1M
#define LEVEL_2_NLIST          512 // 1M
#define LEVEL_3_NLIST          1024 // 10M
#define LEVEL_4_NLIST          4096 // 0.1B
#define LEVEL_5_NLIST          16384 // 1B

#define LEVEL_1_NPROBE         2
#define LEVEL_2_NPROBE         4
#define LEVEL_3_NPROBE         8
#define LEVEL_4_NPROBE         16
#define LEVEL_5_NPROBE         32

#define INIT_NLIST_SCALE       2
#define NLIST                  LEVEL_3_NLIST
#define NPROBE                 LEVEL_1_NPROBE
#define EPSILON                0.05
#define MIN_KMEAN_CLUSTER_SIZE 100 // min update sample in online kmean
#define ALPHA                  0.05 // learning rate for online kmean

#define SAVE_PATH              "/data/saved_rocksdb_faiss/"
#define DELIMITER              "_"
#define IP_PORT                8330
#define IDLE_TIMEOUT_SEC       2000
#define URL                    "brpc_faiss_server"


#define DELETE_APP_ROUTE \
    URL "/delete => delete_server"

#define CREATE_APP_ROUTE \
    URL "/create => create_server,"

#define QUERY_APP_ROUTE \
    URL "/status => get_status_server," \
    URL "/search => search_server," \
    URL "/echo => echo_server" \

#define RESTORE_APP_ROUTE \
    URL "/restore => restore_server"

#define UPDATE_APP_ROUTE \
    URL "/batch_add => batch_add_server"


#endif /* ifndef CONFIG_H */
