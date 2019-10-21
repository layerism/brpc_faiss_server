from locust import HttpLocust, TaskSet, task
import numpy as np
import base64
import json
import uuid
import os
import requests
from collections import deque
from data import *


class WebsiteTasks(TaskSet):

    chunk_id = int(os.environ["CHUNK_IDX"])
    n_split = int(os.environ["N_SPLIT"])
    db_name = os.environ["NAME"]
    file_path = os.environ["FILE_PATH"]

    ann_dataset = BigannData1M(file_path, n_split)
    dim = ann_dataset.dim

    cids, fids, features = ann_dataset.get_chunk(chunk_id)
    cids = deque(cids)
    fids = deque(fids)
    features = deque(features)

    print("#### finish loading, N: {:d}, chunk_size: {:d}".format(chunk_id, len(cids)))

    def on_start(self):
        pass

    @task
    def add(self):

        if (len(self.cids) == 0):
            print("#### finishing add, N {:d}".format(self.chunk_id))
            return

        data = [{
            "cid": self.cids.popleft(),
            "feature_id": self.fids.popleft(),
            "b64_feature": self.features.popleft()
        }]

        request_data = {
            "db_name": self.db_name,
            "feature_version": "v1",
            "feature_dim": self.dim,
            "data": data,
        }

        self.request_data = json.dumps(request_data)
        response = self.client.post("batch_add", data=self.request_data)
        result = json.loads(response.text)


class WebsiteUser(HttpLocust):
    task_set = WebsiteTasks  # 指向TasSet类，定义测试行为
    min_wait = 0
    max_wait = 0
