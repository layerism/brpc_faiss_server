from locust import HttpLocust, TaskSet, task


class WebsiteTasks(TaskSet):

    def on_start(self):
        request_data = {
            "db_name": "random.test"
        }
        self.request_data = json.dumps(request_data)

    @task
    def status(self):
        response = self.client.post("status", data=self.request_data)
        result = json.loads(response.text)


class WebsiteUser(HttpLocust):
    task_set = WebsiteTasks  # 指向TasSet类，定义测试行为
    min_wait = 0
    max_wait = 0
