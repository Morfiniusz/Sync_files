#include <sstream>
#include "ThreadPool.h"


ThreadPool::ThreadPool(int numThreads) : stop(false) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    startTime = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    logWithThreadIdAndTime("ThreadPool", "ThreadPool constructor");
    for (int i = 0; i <= numThreads; i++) {
        logWithThreadIdAndTime("ThreadPool", "Create thread " + std::to_string(i+2));
        threads.emplace_back([this] {
            logWithThreadIdAndTime("ThreadPool", "start");
            while (true) {
                logWithThreadIdAndTime("ThreadPool", "WAIT while");
                Task task;
                { // Zablokowanie sekcji krytycznej
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    logWithThreadIdAndTime("ThreadPool", "WAIT unique_lock");
                    ///wait - oczekuje na sygnał od zmiennej condition
                    ///lambda - warunek sprawdzany przed każdym wybudzeniem
                    ///jeśli warunek jest spełniony to wychodzi z wait
                    ///jeśli warunek nie jest spełniony to odblokowuje mutex i czeka na sygnał
                    logWithThreadIdAndTime("ThreadPool", "WAIT wait");
                    condition.wait(lock, [this] { return stop || !tasks.empty(); });
                    logWithThreadIdAndTime("ThreadPool", "WAIT END");

                    if (stop && tasks.empty()) {
                        return;
                    }

                    logWithThreadIdAndTime("ThreadPool", "move");

                    //zdejmowania z kolejki i przypisywania do zmiennej task
                    task = std::move(tasks.front());

                    logWithThreadIdAndTime("ThreadPool", "pop");
                    //usuwanie z kolejki
                    tasks.pop();

                } // Odblokowanie sekcji krytycznej
                logWithThreadIdAndTime("ThreadPool", "Task execute");
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

void ThreadPool::enqueueTask(std::function<void(std::string)> func, std::string arg) {
    { // Zablokowanie sekcji krytycznej
        ///unique lock - blokuje mutex i odblokowuje go w destruktorze
        ///jest lepszy od lock_guard bo pozwala na odblokowanie mutexa w dowolnym miejscu
        ///queue_mutex, aby tylko jeden wątek mógł uzyskać dostęp do kolejki zadań.
        logWithThreadIdAndTime("Enqueue", "lock");
        std::unique_lock<std::mutex> lock(queue_mutex);
        logWithThreadIdAndTime("Enqueue", "push");
        tasks.emplace(Task{func, arg});
    } // Odblokowanie sekcji krytycznej

    ///powiadomienie o dodaniu zadania do kolejki i odblokowanie mutexa i wybudzenie wątku
    logWithThreadIdAndTime("Enqueue", "notify_one");
    condition.notify_one();
}


void ThreadPool::executeTasks() {
    while (true) {
        logWithThreadIdAndTime("Execute", "lock");
        std::unique_lock<std::mutex> lock(queue_mutex);

        logWithThreadIdAndTime("Execute", "notify_one");
        condition.notify_one();

        while (!stop && tasks.empty()) {
            //Czekanie na zadanie do wykonania i odblokowanie
            logWithThreadIdAndTime("Execute", "wait");
            condition.wait(lock);
        }

        //Jak stop jest true i kolejka jest pusta to zatrzymujemy wątki
        if (stop && tasks.empty()) {
            logWithThreadIdAndTime("Execute", "stop and return");
            return;
        }

        //zdejmowanie pierwszego elementu z kolejki
        logWithThreadIdAndTime("Execute", "notify_one");
        Task task = tasks.front();
        tasks.pop();

        //odblokowanie mutexa przed wykonaniem zadania(tutaj funkcji) aby inne wątki mogły dodać zadania
        logWithThreadIdAndTime("Execute", "unlock");
        lock.unlock();
        logWithThreadIdAndTime("Execute", "execute");
        task.func(task.arg);
    }
}


void ThreadPool::logWithThreadIdAndTime(const std::string &where, const std::string &message) {
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
              "\t" << "id: "<<id_as_ull << "\t" << message << std::endl;
#endif // LOG_ENABLED
}


