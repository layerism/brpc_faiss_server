#!/bin/bash

IP=$1
PORT=$2
JOB=$3

# python config.py --ip ${IP} --port ${PORT} --size 100000

for ((i=1; i<=16; i++))
do
    locust -f ${JOB} --no-web -c 1000 -r 10 --run-time 30s \
        --host="http://${IP}:${PORT}/brpc_faiss_server/" &
done
