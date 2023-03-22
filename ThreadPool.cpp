#include <sstream>
#include "ThreadPool.h"


ThreadPool::ThreadPool(int numThreads) : stop(false) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    startTime = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    threadLogger("ThreadPool", "ThreadPool constructor");
    for (int i = 0; i < numThreads; i++) {
        threadLogger("ThreadPool", "Create thread " + std::to_string(i + 2));
        threads.emplace_back([this] {
            threadLogger("ThreadPool", "start");
            while (true) {
                threadLogger("ThreadPool", "WAIT while");
                Task task;
                { // Zablokowanie sekcji krytycznej
                    threadLogger("ThreadPool", "WAIT unique_lock");
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    threadLogger("ThreadPool", "WAIT wait");
                    condition.wait(lock, [this] { return stop || !tasks.empty(); });
                    threadLogger("ThreadPool", "WAIT END");

                    if (stop && tasks.empty()) {
                        return;
                    }

                    threadLogger("ThreadPool", "move");

                    //zdejmowania z kolejki i przypisywania do zmiennej task
                    task = std::move(tasks.front());

                    threadLogger("ThreadPool", "pop");
                    //usuwanie z kolejki
                    tasks.pop();

                } // Odblokowanie sekcji krytycznej
                threadLogger("ThreadPool", "Task execute");
                task.func(task.arg);
            }
        });
    }

}


ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &thread: threads) {
        thread.join();
    }
}

void ThreadPool::enqueueTask(std::function<void(size_t idx)> func, size_t arg) {
    { // Zablokowanie sekcji krytycznej
        threadLogger("Enqueue", "lock");
        std::unique_lock<std::mutex> lock(queue_mutex);
        threadLogger("Enqueue", "push");
        tasks.emplace(Task{func, arg});
    } // Odblokowanie sekcji krytycznej

    ///powiadomienie o dodaniu zadania do kolejki i odblokowanie mutexa i wybudzenie wątku
    threadLogger("Enqueue", "notify_one");
    condition.notify_one();
}

void ThreadPool::executeTasks() {
    while (true) {
        threadLogger("Execute", "lock");
        std::unique_lock<std::mutex> lock(queue_mutex);

        threadLogger("Execute", "notify_one");
        condition.notify_one();

        while (!stop && tasks.empty()) {
            //Czekanie na zadanie do wykonania i odblokowanie
            threadLogger("Execute", "wait");
            condition.wait(lock);
        }

        //Jak stop jest true i kolejka jest pusta to zatrzymujemy wątki
        if (stop && tasks.empty()) {
            threadLogger("Execute", "stop and return");
            return;
        }

        //zdejmowanie pierwszego elementu z kolejki
        threadLogger("Execute", "notify_one");
        Task task = tasks.front();
        tasks.pop();

        //odblokowanie mutexa przed wykonaniem zadania(tutaj funkcji) aby inne wątki mogły dodać zadania
        threadLogger("Execute", "unlock");
        lock.unlock();
        threadLogger("Execute", "execute");
        task.func(task.arg);
    }
}


void ThreadPool::threadLogger(const std::string &where, const std::string &message) {
    std::stringstream ssWhereMessage;
    ssWhereMessage << where << " " << message;

#ifdef LOG_ENABLED

    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto micro = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    auto sec = (micro / 1000000) - startTime;
    auto millis = (micro % 1000000) / 1000;
    auto micros = micro % 1000;
    std::stringstream ss;
    ss << sec << "." << std::setfill('0') << std::setw(3) << millis << "." << std::setfill('0') << std::setw(3)
       << micros;

    //casting thread id to int
    std::thread::id thread_id = std::this_thread::get_id();
    std::stringstream ssForId;
    ssForId << thread_id;
    unsigned long long id_as_ull;
    ssForId >> id_as_ull;
    std::cout << ss.str() << "\t" << "[" + where + "]" <<
              "\t" << "id: " << id_as_ull << "\t" << message << std::endl;
#endif // LOG_ENABLED
}


