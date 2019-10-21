import numpy as np
import requests
import base64
import json
import argparse


parser = argparse.ArgumentParser(description='test server.')
parser.add_argument('--ip', type=str)
parser.add_argument('--port', type=str, default="8330")
parser.add_argument('--db-name', type=str, default="concurrent.test")
parser.add_argument('--dim', type=int, default=200)
parser.add_argument('--size', type=int, default=100000)
parser.add_argument('--data', type=str)
parser.add_argument('--job', type=str, default="all")
parser.add_argument('--url', type=str, default="http://{:s}:{:s}/brpc_faiss_server/")
args = parser.parse_args()

print(args.url)

QUERY1 = "失败 1bKlPnGT4b4w2O09IVjVvNwunD6itj0+yo2ivNdo0T4Wbnk7JNUnPpdvvT0naxQ+1IBhPjtzzz1cAhA+zZFFvi4Dzj15kXm+odn1vaTGJL79ogQ9DcSCPmDkxb2MSBQ+UpqtPmWmBD7VzJq9sU1Kviv7Jj+M9II+SDVsvApIu70aMwk9hbGFOyfaZT7L8iU+z/eTveyKWT65qfG+cuFQPvAVXbz0UYa9QGm4PmN/Gb30bly+jIE1vcvWwr6Nfgi/PWJ0PA71uz2qYwW+rad2Ps5SMr+6gpU+9fbHPfHYT7z0TZo9oinbPULt5z6SPcI8YvXPPhHIVT5cHFU+SmKpvr9Fv74qN9G82PRwvnh/nD793Tu97Sjmvn7//r4EcNM9A3sMvfJ6MD4Kup2+g8NLvQJm5j5beck9Q+L+PcjSB7z7HnU8dA27Pj82Sbw4aRo+uVD5Pg3FnTwnMjO+ZOWXPGFuG7/JV9q+0zNNvnrerb2eCIq+e8GnvCDt/z2RtpE9AhESPgQ6kz0HPzG//fjrvc5vGLsMA3Y+Zamtvs4X+zzpKUI+I90PPcpskLyPNVO+C9XNvJmCtbz5L9A9/YJtvk8DBr1PBOG+BtSLPhDrDTwFa7y9qgqNvKJ8QT7IQC4+D0dHvq5+LL4jTRy+6srnPrNFUj6FlxA+qwYJv1/Tc76POT++YqMsvEzgtj0yPuy9saWnvdwt8b7tvF2+AWiUvQLZi74Lexq+5zYhvdh/Hb4Fazy+1CrqvjHti74FapE+6rLYvTBL270X9P49OdblPbeZWj718dA9UkgSvny0SD5Epdm+PiPRvWZrDb65AHS+G9mVvfPiVL6vlIW+nfMTvcTuBz/yzTa9orOsPQAAcD7kLkK9fcscPuAO9L08aDY+YhFzPiBhmDxuNes9kC9hvd/8Rj6Y3rY+R3PEPZI7bL0jSfC9o0C/PZ1HRb3/H6c9iDC2vl1SJb5aZqG+c7lRPiHOE75U/Zo+/7PGvV2ozD5L5Y29h2s1PLIRiD43woK+e/fvPv8IQzx7oJU9XW6wPsBci7zsbVO+dNA1vmMLoT0="
QUERY2 = "成功 XhMSvQlOTb6rzX+9fAr4PpErtT2hvxA94lxjPghyED4730+97PekPcqKAT7PgWU+hCzLPXIYTL1jKSK+iZgCvqirOz7erIG+8kDkvhEerb2VYnc+A5fHu8cpWr4eGEA+/bzBPtjyoj5l4Yu+KXa8vpsfVz/w/e09Qlz5PTgsHb6RRgU94LqCvSDPbr57TU8+TpgwPld6jb074Qm/t9VcPgbXvL1DINe9UaWuPq/SvT1ubSG9UBxgPrw9kL6gplK/AHQIPuLnP762MPO+4GmqPiC4Cr/X+429Gt/XPYDzqr4xQ2M9tMyCPqt6WT72I3W+z6TtPSl5NT48MwE+WMl3vjY7Qr613Gm+Mv+Ivnug/T6i66K9Apyevvw0Lr4sSpm+sFbNPRuBOD5dwDu+lX33vaG9Wj55PQg+0sXGPnWTuD2bxiY+o61KPXpt1j0gl5g+J0rCPKYO8j1en6m+hH6ePntK/r7tu4K+uDxGvkWbo74WiAq/E4I1vl5NHjxgPCO+980dPuzCjz57wCy/AB6RPXpRuz0ZWGc+CjCsvEz7Jr73x4u+52/qPnv6qL0j2k6+awsvPnP2zrzXMc69ABp1PnDRiT1iu9u+pP46PR78xL0OiHA+y9clvRSvIr5A+JC9vfzOPKq1oL7+JrS90qysPddsJT48SuU+rkgcv5CeIj0MWji+ZJadvTmYDb3c9Ae+9diWPXno+75hMqW+zlUzPevGo77nxjQ9ou8uvfmeETwMdqO9YhCYvjnvf77ja88+uB0aPhPWdr7jNX8+lShbPspt+zz9o2+9XHKUvqD+Yz55yyW+vw8XvmeBjr7Scxu+8dRDPnL8qL4IILU9cCLaPVD7CT8MzTU+nZ7XPferLD8sgWQ++b+jvdf3wb2eRe89N4wCPPVJzj3FH3U+dvwXPRUAY73jUP89MqrMvRxFhj6ZnNK+oduLPmAhi75K0C8+y6CCvoUkMzwlyVO+JooQPm6mor3DRCO+74y2vUtaYT6GjbK7xEKVPjZXvT6gbiC9z4TePkbt/rxBKqU9hSX+PmH9D74eGEC+rixRPRKhsT0="


def pprint_as_json(result, msg):
    ctx = json.loads(result.text.encode("utf-8"))["ctx"]
    t0 = ctx["start_time"]
    t1 = ctx["end_time"]
    dt = str(float(t1) - float(t0)) + "(ms)"
    print("========== [{:s} server] using: {:s} ==========".format(msg, dt))
    out = json.loads(result.text.encode("utf-8"))
    print(json.dumps(out, indent=4))


def load_data():
    uids = []
    fids = []
    features = []
    with open(args.data, "r", encoding="utf-8") as in_file:
        for i, line in enumerate(in_file):
            uid, feature = line.strip("\n").split(" ", 1)
            uids += [uid]
            fids += ["f" + str(i)]
            features += [feature]
            if i >= args.size:
                break

    return uids, fids, features


def test_status(name):
    request_data = {
        "db_name": name,
    }
    request_data = json.dumps(request_data)
    result = requests.post(url=args.url + "status", data=request_data)
    pprint_as_json(result, "status")
    return result


def test_create(name, dim):
    request_data = {
        "db_name": name,
        "feature_dim": dim,
        "feature_version": "v1",
        "search_type": 0,
        "similarity_type": "L2",
        "search_device": "cpu",
    }
    request_data = json.dumps(request_data)
    print(args.url + "create")
    result = requests.post(url=args.url + "create", data=request_data)
    pprint_as_json(result, "create")
    return result


def test_batch_add(cids, fids, features):
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
    result = requests.post(url=args.url + "batch_add", data=request_data)
    pprint_as_json(result, "batch_add")
    return result


def test_search(query):

    feature = query.split(" ")[1]
    request_data = {
        "db_name": args.db_name,
        "b64_feature": feature,
        "topk": 10,
    }
    request_data = json.dumps(request_data)
    result = requests.post(url=args.url + "search", data=request_data)
    names = map(
        lambda x: (x["cfid"].split("_")[0], x["distance"]),
        json.loads(result.text)["output"]["recall"]
    )
    pprint_as_json(result, "search")
    for name in names:
        print(name)
    return result


if __name__ == "__main__":
    if args.job == "test":
        cids, fids, features = load_data()
        print(len(fids))
        test_create(args.db_name, args.dim)
        test_batch_add(cids, fids, features)
        test_status(args.db_name)
        test_status("")
        test_search(QUERY1)
        test_search(QUERY2)
    elif args.job == 'create':
        test_create(args.db_name, args.dim)
    elif args.job == 'status':
        test_status(args.db_name)
        test_status("")
