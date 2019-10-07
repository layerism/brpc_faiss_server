#!/bin/bash



export OMP_NUM_THREADS=1
mkdir -p /data/saved_rocksdb_faiss
mkdir build
cd build
make clean
cmake ..
make -j8
cd -
cp ./build/*.py ./python/
nohup ./build/brpc_server > /opt/brpc_server/log.txt 2>&1 &
#./build/brpc_server
