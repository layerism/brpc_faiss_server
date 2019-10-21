project(brpc_server C CXX)
cmake_minimum_required(VERSION 3.10.2)


set(LIBS "-lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl -std=c++11 -W -O2 ${LIBS}")
message("===========CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("===========CMAKE_CXX_LIBS: ${LIBS}")


include(FindThreads)
include(FindProtobuf)
message("end of include(Find) function")


find_path(PROTOBUF_INCLUDE_PATH NAMES google/protobuf/service.h)
find_library(PROTOC_LIB NAMES protoc)
if((NOT PROTOBUF_INCLUDE_PATH) OR (NOT PROTOC_LIB))
  message(FATAL_ERROR "Fail to find protoc lib")
endif()
INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_PATH})
message("===========PROTOBUF_INCLUDE_PATH: ${PROTOBUF_INCLUDE_PATH}")


protobuf_generate_cpp(PROTO_SRC PROTO_HEADER protobuf/basic_server.proto)
protobuf_generate_python(PROTO_PYS protobuf/basic_server.proto)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
message("===========CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message("===========PROTO_SRC: ${PROTO_SRC}")
message("===========PROTO_HEADER: ${PROTO_HEADER}")
message("===========PROTO_PYS: ${PROTO_PYS}")


find_path(GFLAGS_INCLUDE_PATH gflags/gflags.h)
find_library(GFLAGS_LIBRARY NAMES gflags libgflags)
if((NOT GFLAGS_INCLUDE_PATH) OR (NOT GFLAGS_LIBRARY))
    message(FATAL_ERROR "Fail to find gflags")
endif()
INCLUDE_DIRECTORIES(${GFLAGS_INCLUDE_PATH})
message("===========GFLAGS_LIBRARY: ${GFLAGS_LIBRARY}")
message("===========GFLAGS_INCLUDE_PATH: ${GFLAGS_INCLUDE_PATH}")


find_path(LEVELDB_INCLUDE_PATH NAMES leveldb/db.h)
find_library(LEVELDB_LIB NAMES leveldb)
if ((NOT LEVELDB_INCLUDE_PATH) OR (NOT LEVELDB_LIB))
    message(FATAL_ERROR "Fail to find leveldb")
endif()
INCLUDE_DIRECTORIES(${LEVELDB_INCLUDE_PATH})
message("===========LEVELDB_INCLUDE_PATH: ${LEVELDB_INCLUDE_PATH}")


find_library(SSL_LIB NAMES ssl)
if (NOT SSL_LIB)
    message(FATAL_ERROR "Fail to find ssl")
endif()


find_library(CRYPTO_LIB NAMES crypto)
if (NOT CRYPTO_LIB)
    message(FATAL_ERROR "Fail to find crypto")
endif()


SET(TARGET "demo")
SET(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third_party/)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/utils/)
INCLUDE_DIRECTORIES(${THIRD_PARTY_DIR}/)
INCLUDE_DIRECTORIES(${THIRD_PARTY_DIR}/incubator-brpc/include)
INCLUDE_DIRECTORIES(${THIRD_PARTY_DIR}/faiss/include)
INCLUDE_DIRECTORIES(${THIRD_PARTY_DIR}/rocksdb/include)
message("===========CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("===========THIRD_PARTY_DIR: ${THIRD_PARTY_DIR}")


set(DYNAMIC_LIB
    ${CMAKE_THREAD_LIBS_INIT}
    ${GFLAGS_LIBRARY}
    ${PROTOBUF_LIBRARIES}
    ${LEVELDB_LIB}
    ${SSL_LIB}
    ${CRYPTO_LIB}
    ${JSONCPP_LIB}
    ${THIRD_PARTY_DIR}/incubator-brpc/lib/libbrpc.a
    ${THIRD_PARTY_DIR}/faiss/lib/libfaiss.so
    ${THIRD_PARTY_DIR}/rocksdb/lib/librocksdb.so
    #${THIRD_PARTY_DIR}/rocksdb/lib/librocksdb.so.6.4.0
    pthread
)


if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(DYNAMIC_LIB ${DYNAMIC_LIB}
        pthread
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework CoreData"
        "-framework CoreText"
        "-framework Security"
        "-framework Foundation"
        "-Wl,-U,_MallocExtension_ReleaseFreeMemory"
        "-Wl,-U,_ProfilerStart"
        "-Wl,-U,_ProfilerStop")
endif()
message("===========DYNAMIC_LIB: ${DYNAMIC_LIB}")


set(AUX_SRC
    ${CMAKE_SOURCE_DIR}/src/utils/cstring.cc
    ${CMAKE_SOURCE_DIR}/src/utils/base64.cc
    ${CMAKE_SOURCE_DIR}/src/utils/log.cc
    ${CMAKE_SOURCE_DIR}/src/tensor/linalg.cc
    ${CMAKE_SOURCE_DIR}/src/database.cc
    ${CMAKE_SOURCE_DIR}/src/faiss_index.cc
    ${CMAKE_SOURCE_DIR}/src/interfaces.cc
    ${CMAKE_SOURCE_DIR}/src/io_rocksdb.cc
    ${CMAKE_SOURCE_DIR}/src/server_query.cc
    ${CMAKE_SOURCE_DIR}/src/server_create.cc
    ${CMAKE_SOURCE_DIR}/src/server_update.cc
    ${CMAKE_SOURCE_DIR}/src/server_delete.cc
    ${CMAKE_SOURCE_DIR}/src/server_restore.cc
)
add_executable(brpc_server src/run_server.cc ${PROTO_SRC} ${PROTO_PYS} ${AUX_SRC})
TARGET_LINK_LIBRARIES(brpc_server ${DYNAMIC_LIB})
