from locust import HttpLocust, TaskSet, task
import numpy as np
import base64
import json


class WebsiteTasks(TaskSet):

    def on_start(self):
        feature = np.random.normal(0.0, 0.1, 200).astype(np.float32)
        feature = feature / np.linalg.norm(feature, ord=2, axis=0)
        feature = base64.b64encode(feature.tobytes())
        feature = feature.decode("utf-8", "ignore")
        request_data = {
            "db_name": "random.test",
            "b64_feature": feature,
            "topk": 10,
        }
        self.request_data = json.dumps(request_data)

    @task
    def search(self):
        response = self.client.post("search", data=self.request_data)
        result = json.loads(response.text)


class WebsiteUser(HttpLocust):
    task_set = WebsiteTasks  # 指向TasSet类，定义测试行为
    min_wait = 0
    max_wait = 0
