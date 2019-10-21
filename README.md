# 基于 brpc + faiss 的矢量检索框架


## 概述
矢量检索，顾名思义，就是通过固定维度的浮点数矢量，在数据库里查找与之距离最近的 top-k 个矢量的过程，在深度学习领域是一个非常常见，而且非常实用的基础组件，在视频检索，文本召回，人脸识别验证，推荐系统等问题均有很多应用。

这里，我们提供一个阉割版的矢量检索服务框架，性能并不高，主要用于教学目的，不过可以基于此框架升级到比较实用的状态。

阉割版目前的特性包括：

1. 支持并发的增删改查过程，查询 QPS 是上万级
2. 采用一种特殊的随机过程对分桶函数进行 online 训练，所以可以支持实时 + 并发的入库，以 online 方式构建索引，自动索引下，写入 QPS 约为 7000-8000
3. 支持简单的持久化方案
4. 提供一些简单的并发测试脚本
5. 单机上限差不多是亿级，不过对系统资源占用极大，阉割版没有提供量化，以及多索引方案，对系统资源占用也没有限制，容易出现问题，不建议加载这种量级数据。

性能评测 (测试机器为 32 核 cpu 机器，内存 60 G)：

1. 200 维 x 10 M 数据，写入 QPS 约为 6000，检索 QPS 约为 2000-3000
2. 128 维 x 1M 数据，写入 QPS 约为 8000，检索 QPS 约为 8000



## 启动服务

>  确保系统安装有 docker，并保证在代码的根目录下运行如下命令：

1. 构建镜像
```bash
TARGET=brpc_faiss_server:v1
docker build -t ${TARGET} .
```

2. 启动镜像，比如我们设置端口为 8330，需要将 src/config.h 里面的 IP_PORT 修改为 8330，数据缓存位置设置为 tmp，如有需要可以换到你想要的任意目录下
```bash
SERVER_TMP_DIR=/absolute/path/to/your/tmp/dir
mkdir ${SERVER_TMP_DIR}
docker run -it --runtime nvidia --name brpc_faiss_server -v ${SERVER_TMP_DIR}:/data/saved_rocksdb_faiss -v `pwd`:/opt/brpc_server -p 8330:8330 ${TARGET} /bin/bash
```

3. 启动服务

```bash
docker exec brpc_faiss_server /bin/sh -c /opt/brpc_server/start.sh
```



## 使用服务

1. 获取 docker 的宿主机 ip 地址 xx.xxx.x.xxx，也就是服务地址
2. 内部提供 protobuf 的接口，不过为了方便展示，这里用 http 方式调度
3. python 可以直接使用 request 模块进行 http 方式调度服务，也可以使用 curl 命令访问
4. 服务监控面板，可以在浏览器打开 http://xx.xxx.x.xxx:8330/status ，进行监控，这里的  xx.xxx.x.xxx  就是宿主机 ip



提供了 6 个服务，每个服务的 url 为：

> 创建一个检索数据库： http://xx.xxx.x.xxx:8330/brpc_faiss_server/create
>
> 批量添加数据：http://xx.xxx.x.xxx:8330/brpc_faiss_server/batch_add
>
> 删除数据库：http://xx.xxx.x.xxx:8330/brpc_faiss_server/delete
>
> 查询数据库状态：http://xx.xxx.x.xxx:8330/brpc_faiss_server/status
>
> 从指定位置，恢复数据库：http://xx.xxx.x.xxx:8330/brpc_faiss_server/restore
>
> 进行矢量检索：http://xx.xxx.x.xxx:8330/brpc_faiss_server/search



增加一个库：

```python
import requests
url = " http://xx.xxx.x.xxx:8330/brpc_faiss_server/create"
request_data = {
	"db_name": "word.emb",
	"feature_dim": 200,
	"feature_version": "v1",
	"search_type": 0,
	"similarity_type": "L2",
	"search_device": "cpu",
}
request_data = json.dumps(request_data)
result = requests.post(url=url, data=request_data)
```



查询服务状态：

```python
import requests
url = " http://xx.xxx.x.xxx:8330/brpc_faiss_server/status"
request_data = {
	"db_name": "word.emb",
}
request_data = json.dumps(request_data)
result = requests.post(url=url, data=request_data)
```



删除某个库：

```python
import requests
url = " http://xx.xxx.x.xxx:8330/brpc_faiss_server/delete"
request_data = {
	"db_name": "word.emb"
}
request_data = json.dumps(request_data)
result = requests.post(url=url, data=request_data)
```



批量添加：

```python
import requests
url = " http://xx.xxx.x.xxx:8330/brpc_faiss_server/batch_add"
data = []
for cid, fid, feature in list(zip(cids, fids, features)):
	item = {"cid": cid, "feature_id": fid, "b64_feature": feature}
	data.append(item)

request_data = {
	"db_name": "word.emb",
	"feature_version": "v1",
	"feature_dim": 200,
	"data": data,
}

request_data = json.dumps(request_data)
result = requests.post(url=url, data=request_data)
```



矢量检索：

```python
import requests
url = " http://xx.xxx.x.xxx:8330/brpc_faiss_server/search"
request_data = {
	"db_name": "word.emb",
    "b64_feature": "NxxWPhZuKb4Iym29TfaPPjro0j2Yh8w9A7BBPfGC6D0drE8+6kB2P...",
    "topk": 10,
}
request_data = json.dumps(request_data)
result = requests.post(url=url, data=request_data)
```





## 并发测试

这里只是提供简单的 python 接口做并发测试，机器： 32 核 cpu 机器

首先安装的 python 库，其中 Locust 库是并发压测工具，可以依靠单机多核，轻松形成几万的 QPS：

```python
pip install requests, Locust, numpy
```

下载 bigann 1M 数据集：ftp://ftp.irisa.fr/local/texmex/corpus/sift.tar.gz，并解压到用户指定目录里面

cd 到 python 的 test_concurrent 目录里面

创建一个库，并执行并发入库：

```sh
./run.sh xx.xxx.x.xxx add.py 3m path_to_bigann
```

检索：

```sh
./run.sh xx.xxx.x.xxx search.py 3m path_to_bigann
```



在压测过程，在浏览器打开 http://xx.xxx.x.xxx:8330/status ，就可以查看性能面板，非常方便：

![1570446682487](image/1570446682487.png)





## 动态索引方案

定义 $k$ 个粗中心点 $c_i$，以及分桶函数 $p_c(i|x)$：
$$
p_c(i|x) = \frac{e^{-\eta |x-c_i|^2}}{\sum_j e^{-\eta |x-c_j|^2}}
$$
给样本划分桶的时候，采取 $\arg\max_i p_c(i|x)$ 方式选择桶，当 $\eta$ 无穷大的时候，分桶函数就是最近邻分桶。

对于 faiss 来讲，分桶实现方式是构建 voronoi cell，方法是 kmean，每个 cell 都是一个桶，cell 内的样本都是桶内样本，不同 cell 之间的样本是桶间样本。然而，voronoi cell 本身有一定缺点：

1. 有一定可能出现大量样本落在两个 cell 之间的边界上，导致检索需要跨桶判断边界样本，增加时间开销
2. 某个 cell 里面包含太多样本，导致检索某个桶的负担增加

我们提出，对于倒排矢量检索来讲，一个好的分桶策略应当满足如下要求：

1. 每个桶内的样本距离桶中心的偏差都不大，数学表述为 $ \sum_x |x-c_i|^2$ 要小
2. 桶间样本的距离要拉大

这样，检索过程只要在单个桶里检索便可以，

给定一个数据分布 $p(x)$，我们希望每个样本距离自己最近的中心点更近，距离剩余中心点更远：
$$
l(x)=\sum_i |x-c_i|^2 p(i|x) + \sum_i |x-c_i|^2 (1-p(i|x))
$$
