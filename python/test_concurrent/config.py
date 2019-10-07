import numpy as np
import requests
import base64
import json
import argparse


parser = argparse.ArgumentParser(description='test server.')
parser.add_argument('--ip', type=str)
parser.add_argument('--port', type=str)
parser.add_argument('--db-name', type=str, default="random.test")
parser.add_argument('--dim', type=int, default=200)
parser.add_argument('--size', type=int, default=100000)
args = parser.parse_args()

HOST = "http://{:s}:{:s}/brpc_faiss_server/".format(args.ip, args.port)


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
        cids += [names[i % len(names)]]
        fids += ["f" + str(i)]
        mean = float(i // n_cluster)
        feature = np.random.normal(mean, 0.1, args.dim).astype(np.float32)
        #feature = feature / np.linalg.norm(feature, ord=2, axis=0)
        feature = base64.b64encode(feature.tobytes())
        features += [feature.decode("utf-8", "ignore")]

    return cids, fids, features
cids, fids, features = gen_data()
print(len(fids))


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
    result = requests.post(url=HOST + "create", data=request_data)
    return result


def test_batch_add():
    data = []
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
    result = requests.post(url=HOST + "batch_add", data=request_data)
    return result


if __name__ == "__main__":
    pprint_as_json(test_create(), "create")
    pprint_as_json(test_batch_add(), "batch_add")
