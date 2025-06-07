#include "pch.h"
#include "TaskManager.h"

TaskManager::TaskManager() {
    Start();
}

TaskManager::~TaskManager() {
    Stop();
}

void TaskManager::Start() {
    unsigned int threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 4; // fallback

    for (unsigned int i = 0; i < threadCount; ++i) {
        workers.emplace_back(&TaskManager::Worker, this);
    }
}

void TaskManager::Stop() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();

    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers.clear();
}

void TaskManager::Worker() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });

            if (stop && tasks.empty()) return;

            task = std::move(tasks.front());
            tasks.pop();
        }

        task();
    }
}