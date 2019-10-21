#!/bin/bash

IP=$1
PORT=8330
JOB=$2
TIME=$3
DATA_PATH=$4

HOST="http://${IP}:${PORT}/brpc_faiss_server/"
N=25
N_USERS=1000000
RATE=10
DIM=128
DB_NAME="concurrent.test"


python config.py \
    --ip ${IP} \
    --port ${PORT} \
    --dim ${DIM} \
    --url ${HOST} \
    --job "create" \
    --db-name ${DB_NAME}


env CHUNK_IDX=0 \
env N_SPLIT=${N} \
env FILE_PATH=${DATA_PATH} \
env NAME=${DB_NAME} \
locust -f ${JOB} --no-web -c ${N_USERS} -r ${RATE} -t ${TIME} --master --host=${HOST} &
for ((i=1; i<${N}; i++))
do
{
    env CHUNK_IDX=${i} \
    env N_SPLIT=${N} \
    env FILE_PATH=${DATA_PATH} \
    env NAME=${DB_NAME} \
    locust -f ${JOB} --no-web -c ${N_USERS} -r ${RATE} --slave --host=${HOST}
}&
done
wait

python config.py \
    --ip ${IP} \
    --port ${PORT} \
    --dim ${DIM} \
    --url ${HOST} \
    --job "status" \
    --db-name ${DB_NAME}


# locust -f ${JOB} --no-web -c 1000 -r 10 --run-time 10s \
#      --host="http://${IP}:${PORT}/PerceptionService/SimilaritySearch/"
# ps -ef | grep "locust -f" | awk '{print $2}' | xargs kill -s 9
