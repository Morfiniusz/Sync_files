#include "ThreadManager.h"

ThreadPool::ThreadPool(int numThreads) : stop(false) {
    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([this] {
            while (true) {
                Task task;
                { // Zablokowanie sekcji krytycznej
                    unique_lock<mutex> lock(queue_mutex);
                    condition.wait(lock, [this] { return stop || !tasks.empty(); });

                    if (stop && tasks.empty()) {
                        return;
                    }

                    //zdejmowania z kolejki i przypisywania do zmiennej task
                    task = std::move(tasks.front());
                    //usuwanie z kolejki
                    tasks.pop();

                } // Odblokowanie sekcji krytycznej
                task.func(task.arg);
            }
        });
    }
}

void ThreadPool::enqueueTask(function<void(string)> func, string arg) {
    { // Zablokowanie sekcji krytycznej
        unique_lock<mutex> lock(queue_mutex);
        tasks.emplace(Task{func, arg});
    } // Odblokowanie sekcji krytycznej
    condition.notify_one();
}