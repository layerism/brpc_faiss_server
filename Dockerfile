FROM nvidia/cuda:9.0-cudnn7-devel-centos7

WORKDIR /opt/
WORKDIR /opt/third_party

RUN mkdir -p /opt/third_party/faiss \
    && mkdir -p /opt/third_party/incubator-brpc \
    && mkdir -p /opt/third_party/rocksdb \
    && mkdir -p /data/saved_rocksdb_faiss


# Install necessary build tools
RUN yum install -y wget epel-release git gcc-c++ make swig3 rsync \
    numpy python-devel vim curl gmake unzip  \
    libpcap bzip2 sysstat google-perftools htop nohup


# install anaconda3
RUN wget https://repo.continuum.io/archive/Anaconda3-5.3.0-Linux-x86_64.sh -O /opt/anaconda3.sh \
    && echo 'export PATH=/opt/conda/bin:$PATH' > /etc/profile.d/conda.sh \
    && /bin/bash /opt/anaconda3.sh -b -p /opt/conda \
    && export PATH=/opt/conda/bin:$PATH \
    && conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/ \
    && conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main/ \
    && conda config --set show_channel_urls yes \
    && conda update -y pip \
    && conda install -y cython=0.29.13 \
    && rm -rf /opt/anaconda3.sh \
    && conda clean -y -a
ENV PATH=/opt/conda/bin:$PATH
ENV CPATH=/usr/include:$CPATH
ENV LD_LIBRARY_PATH=/opt/conda/lib:$LD_LIBRARY_PATH
ENV LIBRARY_PATH=/usr/lib64:/opt/conda/lib:$LIBRARY_PATH
ENV LD_LIBRARY_PATH=/usr/lib64:/usr/local/lib:/opt/conda/lib:$LD_LIBRARY_PATH
ENV LD_PRELOAD=/usr/lib64/libgomp.so.1:$LD_PRELOAD
ENV LD_PRELOAD=/opt/conda/lib/libmkl_def.so:$LD_PRELOAD
ENV LD_PRELOAD=/opt/conda/lib/libmkl_avx2.so:$LD_PRELOAD
ENV LD_PRELOAD=/opt/conda/lib/libmkl_core.so:$LD_PRELOAD
ENV LD_PRELOAD=/opt/conda/lib/libmkl_intel_lp64.so:$LD_PRELOAD
ENV LD_PRELOAD=/opt/conda/lib/libmkl_gnu_thread.so:$LD_PRELOAD



# Install incubator-brpc
RUN yum install -y gflags-devel protobuf-devel protobuf-compiler \
    leveldb-devel gdb gdb-gdbserver snappy openssl-devel \
    && git clone https://github.com/apache/incubator-brpc.git \
    && cd /opt/incubator-brpc \
    && sh config_brpc.sh --headers=/usr/include --libs=/usr/lib64 \
    && make -j "$(nproc)" \
    && mv /opt/incubator-brpc/output/* /opt/third_party/incubator-brpc/ \
    && rm -rf /opt/incubator-brpc*



# install faiss-gpu
RUN wget https://github.com/facebookresearch/faiss/archive/v1.5.3.tar.gz -O /opt/faiss.tar \
    && cd /opt \
    && tar -xvzf faiss.tar \
    && mv faiss-1.5.3 faiss \
    && cd /opt/faiss \
    && mkdir -p /opt/faiss/build \
    && ./configure --prefix=/opt/faiss/build --with-cuda-arch="\
        -gencode=arch=compute_52,code=compute_52 \
        -gencode=arch=compute_60,code=sm_60 \
        -gencode=arch=compute_70,code=sm_70" \
        --with-python=python3.7 \
        --with-cuda=/usr/local/cuda-9.0 \
    && make -j "$(nproc)" \
    && make py \
    && make install \
    && mv /opt/faiss/build/* /opt/third_party/faiss/ \
    && mv /opt/faiss/python /opt/third_party/faiss/ \
    && rm -rf /opt/faiss*
ENV PYTHONPATH=/opt/third_party/faiss/python:$PYTHONPATH



# Install RocksDB
RUN yum install -y snappy snappy-devel zlib zlib-devel bzip2 bzip2-devel lz4-devel libasan \
    && wget https://github.com/facebook/rocksdb/archive/v6.2.4.tar.gz -O /opt/rocksdb.tar \
    && wget https://github.com/facebook/zstd/archive/v1.1.3.tar.gz -O /opt/zstd.tar \
    && cd /opt/ \
    && tar -xvzf /opt/rocksdb.tar \
    && tar -xvzf /opt/zstd.tar \
    && cd /opt/zstd-1.1.3 \
    && make \
    && make install \
    && export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH \
    && cd /opt/rocksdb-6.2.4 \
    && make shared_lib -j "$(nproc)" \
    && mv /opt/rocksdb-6.2.4/include /opt/third_party/rocksdb/ \
    && mkdir -p /opt/third_party/rocksdb/lib \
    && mv /opt/rocksdb-6.2.4/librocksdb.so* /opt/third_party/rocksdb/lib/ \
    && rm -rf /opt/rocksdb* /opt/zstd*
ENV CPLUS_INCLUDE_PATH=/opt/third_party/rocksdb/include:$CPLUS_INCLUDE_PATH
ENV LD_LIBRARY_PATH=/opt/third_party/rocksdb/lib:$LD_LIBRARY_PATH
ENV LIBRARY_PATH=/opt/third_party/rocksdb/lib:$LIBRARY_PATH



# install python-rocksdb
RUN wget https://github.com/twmht/python-rocksdb/archive/master.zip \
    && cd /opt \
    && unzip master.zip \
    && mv /opt/python-rocksdb-master /opt/python-rocksdb \
    && cd /opt/python-rocksdb \
    && python setup.py build \
    && mkdir -p /opt/third_party/rocksdb/python \
    && mv build/lib.linux-x86_64-3.7/* /opt/third_party/rocksdb/python \
    && rm -rf /opt/python-rocksdb* /opt/master*
ENV PYTHONPATH=/opt/third_party/rocksdb/python:$PYTHONPATH


# Install cmake-3.10.2
RUN wget https://cmake.org/files/v3.10/cmake-3.10.2.tar.gz -O /opt/cmake.tar \
    && cd /opt/ \
    && tar -xzvf cmake.tar \
    && cd /opt/cmake-3.10.2 \
    && ./bootstrap \
    && gmake -j "$(nproc)" \
    && make install \
    && ln -s /usr/local/bin/cmake /usr/bin/cmake \
    && rm -rf /opt/cmake*


# WORKDIR /opt/brpc_server
# COPY . /opt/brpc_server/
# RUN cd /opt/brpc_server && ln -s ../third_party ./
