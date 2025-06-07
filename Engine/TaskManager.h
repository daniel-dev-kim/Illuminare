// TaskManager.h
#pragma once
#include "pch.h"

class TaskManager {
public:
    TaskManager();
    ~TaskManager();

    void Start();
    void Stop();

    template<typename F>
    auto Enqueue(F&& task) -> std::future<decltype(task())> {
        using ReturnType = decltype(task());

        auto packagedTask = std::make_shared<std::packaged_task<ReturnType()>>(std::forward<F>(task));
        auto future = packagedTask->get_future();

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            tasks.emplace([packagedTask]() { (*packagedTask)(); });
        }

        condition.notify_one();
        return future;
    }

private:
    void Worker();

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;
};
