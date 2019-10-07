import threading
import requests
import json
import numpy as np
import random
import string
import base64
import time
import argparse


parser = argparse.ArgumentParser(description='test server.')
parser.add_argument('--ip', type=str)
parser.add_argument('--port', type=str)
parser.add_argument('--job', type=str)
parser.add_argument('--db-name', type=str, default="word.emb")
parser.add_argument('--nthread', type=int, default=1)
parser.add_argument('--data', type=str, default="./bin/word_embedding/tencent-ai.word.emb.float32.base64")
parser.add_argument('--dim', type=int, default=200)
parser.add_argument('--size', type=int, default=100000)
parser.add_argument('--repeat', type=int, default=200)
args = parser.parse_args()


URL = "http://{:s}:{:s}/brpc_faiss_server/".format(args.ip, args.port)


def pprint_as_json(result, msg):
    ctx = json.loads(result.text.encode("utf-8"))["ctx"]
    t0 = ctx["start_time"]
    t1 = ctx["end_time"]
    dt = str(float(t1) - float(t0)) + "(ms)"
    print("========== [{:s} server] using: {:s} ==========".format(msg, dt))
    out = json.loads(result.text.encode("utf-8"))
    print(json.dumps(out, indent=4))


def gen_data(n_cluster=1000):
    names = list(map(lambda x: "usr:" + str(x), range(n_cluster)))
    cids = []
    fids = []
    features = []
    for i in range(args.size):
        cids += [np.random.choice(names)]
        fids += ["f" + str(i)]
        mean = float(i // n_cluster)
        feature = np.random.normal(mean, 0.1, args.dim).astype(np.float32)
        feature = feature / np.linalg.norm(feature, ord=2, axis=0)
        feature = base64.b64encode(feature.tobytes())
        features += [feature.decode("utf-8", "ignore")]

    return cids, fids, features


def load_data():
    cids = []
    fids = []
    features = []
    with open(args.data, "r") as in_file:
        for i, line in enumerate(in_file):
            cid, b64_feature = line.strip("\n").split(" ", 1)
            cids += [cid]
            fids += ["f" + str(i)]
            features += [b64_feature]
    return cids, fids, features
cids, fids, features = gen_data()
print(len(fids))


class wrapper_multi_thread(threading.Thread):

    def __init__(self, func, idx):
        threading.Thread.__init__(self)
        self.idx = idx
        self.func = func
        self.result = None
        self.time_spend = None

    def run(self):
        t0 = time.time()
        for i in range(args.repeat):
            self.result = self.func(self.idx)
        t1 = time.time()
        self.time_spend = (t1 - t0)


class run_multi_thread(object):

    def __init__(self, func, n_thread=10):
        self.func = func
        self.thread = []
        self.n_thread = n_thread
        for i in range(self.n_thread):
            self.thread.append(wrapper_multi_thread(func, i))

    def run(self):
        result = []
        for i in range(self.n_thread):
            self.thread[i].start()

        for i in range(self.n_thread):
            self.thread[i].join()

        for i in range(self.n_thread):
            result.append(self.thread[i].result)

        mp_time_spend = [mp.time_spend * 1000 / args.repeat for mp in self.thread]
        time_spend = sum(mp_time_spend)
        print("\n")
        print("======================================")
        print("task: {:s}".format(self.func.__name__))
        print("mean process time (ms): ", self.n_thread * args.repeat / time_spend)
        print("min-max(ms): ", min(mp_time_spend), max(mp_time_spend))

        # pprint_as_json(, str(self.func))
        return result[np.random.choice(self.n_thread)]


def test_create():
    request_data = {
        "db_name": args.db_name,
        "feature_dim": args.dim,
        "feature_version": "v1",
        "search_type": 0,
        "similarity_type": "L2",
        "search_device": "cpu",
    }
    request_data = json.dumps(request_data)
    result = requests.post(url=URL + "create", data=request_data)
    return result


def test_delete():
    request_data = {
        "db_name": args.db_name
    }
    request_data = json.dumps(request_data)
    result = requests.post(url=URL + "delete", data=request_data)
    return result


def test_get_status(idx):
    request_data = {
        "db_name": args.db_name,
    }
    request_data = json.dumps(request_data)
    result = requests.post(url=URL + "status", data=request_data)
    return result


def test_batch_add():
    data = []
    # for cid, fid, feature in list(zip(cids, fids, features))[idx::args.nthread]:
    for cid, fid, feature in list(zip(cids, fids, features)):
        item = {"cid": cid, "feature_id": fid, "b64_feature": feature}
        data.append(item)

    request_data = {
        "db_name": args.db_name,
        "feature_version": "v1",
        "feature_dim": args.dim,
        "data": data,
    }

    request_data = json.dumps(request_data)
    result = requests.post(url=URL + "batch_add", data=request_data)
    return result


def test_search(idx):
    request_data = {
        "db_name": args.db_name,
        "b64_feature": features[idx],
        "topk": 10,
    }
    request_data = json.dumps(request_data)
    result = requests.post(url=URL + "search", data=request_data)
    return result


def test_echo(idx):
    request_data = {
        "db_name": args.db_name,
    }
    request_data = json.dumps(request_data)
    result = requests.post(url=URL + "echo", data=request_data)
    return result


if __name__ == "__main__":
    if args.job == "create":
        status = test_create()
    elif args.job == "delete":
        status = test_delete()
    elif args.job == "batch_add":
        impl = run_multi_thread(test_batch_add, args.nthread)
        status = impl.run()
    elif args.job == "status":
        impl = run_multi_thread(test_get_status, args.nthread)
        status = impl.run()
    elif args.job == "search":
        impl = run_multi_thread(test_search, args.nthread)
        status = impl.run()
    elif args.job == "echo":
        impl = run_multi_thread(test_echo, args.nthread)
        status = impl.run()
    elif args.job == "all":
        pprint_as_json(test_create(), "create")

        pprint_as_json(test_batch_add(), "batch_add")

        impl = run_multi_thread(test_echo, args.nthread)
        pprint_as_json(impl.run(), "echo")

        impl = run_multi_thread(test_get_status, args.nthread)
        pprint_as_json(impl.run(), "get_status")

        impl = run_multi_thread(test_search, args.nthread)
        pprint_as_json(impl.run(), "test_search")

        pprint_as_json(test_delete(), "delete")

    if args.job != "all":
        pprint_as_json(status, args.job)
