import numpy as np
import os
import base64
import requests
import requests_ftp
import uuid
from tqdm import tqdm


class BigannData1M(object):

    def __init__(self, file_path, n_split=1000, total=1000000, dim=128):
        self.base_fvecs = os.path.join(file_path, "sift/sift_base.fvecs")
        self.query_fvecs = os.path.join(file_path, "sift/sift_query.fvecs")
        self.groundtruth = os.path.join(file_path, "sift/sift_groundtruth.ivecs")

        self.file_path = file_path
        self.n_split = n_split
        self.total = total
        self.dim = dim
        self.topk = 100
        self.chunks = np.array_split(range(total), n_split)

        os.system("mkdir -p {:s}".format(file_path))
        if not os.path.exists('{:s}/sift.tar.gz'.format(file_path)):
            print("==== downloading =====")
            self.download()
            print("==== unzip =====")
            cmd = "tar -xzvf {fp:s}/sift.tar.gz -C {fp:s}/".format(fp=file_path)
            os.system(cmd)

    def download(self):
        pass

    def get_groundtruth(self):
        topk_ids = []
        with open(self.groundtruth, "rb") as ifile:
            while True:
                d = ifile.read(4)
                ids = ifile.read(4 * self.topk)
                if not ids:
                    break
                topk_ids.append(np.frombuffer(ids, dtype=np.int32))
        return topk_ids

    def get_chunk(self, i):
        length = len(self.chunks[i])
        min_idx = np.min(self.chunks[i])
        max_idx = np.max(self.chunks[i])
        cids, fids, features = [], [], []
        start = (4 + 4 * self.dim) * min_idx
        count = min_idx
        with open(self.base_fvecs, "rb") as ifile:
            ifile.seek(start, 0)
            while True:
                d = ifile.read(4)
                vec = ifile.read(4 * self.dim)
                if not vec:
                    break
                cids.append(str(count))
                fids.append(str(uuid.uuid1()))
                feature = base64.b64encode(vec)
                feature = feature.decode("utf-8", "ignore")
                features.append(feature)
                if len(cids) == length:
                    break
                count += 1
        return cids, fids, features

    def get_query(self):
        cids, fids, features = [], [], []
        count = 0
        with open(self.query_fvecs, "rb") as ifile:
            while True:
                d = ifile.read(4)
                vec = ifile.read(4 * self.dim)
                if not vec:
                    break
                cids.append(str(count))
                fids.append(str(uuid.uuid1()))
                feature = base64.b64encode(vec)
                feature = feature.decode("utf-8", "ignore")
                features.append(feature)
                count += 1
        return cids, fids, features


if __name__ == "__main__":
    ann_dataset = BigannData1M("./bigann", n_split=3)
    # cids, fids, features = ann_dataset.get_chunk(0)
    # print(len(cids))

    # cids, fids, features = ann_dataset.get_query()
    # gt_ids = ann_dataset.get_groundtruth()
    #
    # print(gt_ids)
