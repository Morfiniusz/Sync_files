#include "ThreadManager.h"


ThreadManager::ThreadManager(int numThreads): stop(false) {
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
                        task = move(tasks.front());
                        //usuwanie z kolejki
                        tasks.pop();

                    } // Odblokowanie sekcji krytycznej
                    task.func(task.arg);
                }
            });
        }
    }


