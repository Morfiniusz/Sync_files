#include <sstream>
#include "ThreadPool.h"


ThreadPool::ThreadPool(int numThreads) : stop(false) {
    logWithThreadIdAndTime("Thread", "ThreadPool constructor");
    for (int i = 0; i <= numThreads; i++) {
        logWithThreadIdAndTime("Thread", "Create thread");
        threads.emplace_back([this] {
            logWithThreadIdAndTime("Thread", "start");
            while (true) {
                logWithThreadIdAndTime("Thread", "WAIT while");
                Task task;
                { // Zablokowanie sekcji krytycznej
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    logWithThreadIdAndTime("Thread", "WAIT unique_lock");
                    ///wait - oczekuje na sygnał od zmiennej condition
                    ///lambda - warunek sprawdzany przed każdym wybudzeniem
                    ///jeśli warunek jest spełniony to wychodzi z wait
                    ///jeśli warunek nie jest spełniony to odblokowuje mutex i czeka na sygnał
                    logWithThreadIdAndTime("Thread", "WAIT wait");
                    condition.wait(lock, [this] { return stop || !tasks.empty(); });

                    logWithThreadIdAndTime("Thread", "WAIT END");

                    if (stop && tasks.empty()) {
                        return;
                    }

                    logWithThreadIdAndTime("Thread", "move");

                    //zdejmowania z kolejki i przypisywania do zmiennej task
                    task = std::move(tasks.front());

                    logWithThreadIdAndTime("Thread", "pop");
                    //usuwanie z kolejki
                    tasks.pop();

                } // Odblokowanie sekcji krytycznej
                logWithThreadIdAndTime("Thread", "Task execute");
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
        std::cout << "[Execute] lock" << std::endl;
        std::unique_lock<std::mutex> lock(queue_mutex);

        std::cout << "[Execute] notify_one !!!" << std::endl;
        condition.notify_one();

        while (!stop && tasks.empty()) {
            //Czekanie na zadanie do wykonania i odblokowanie
            std::cout << "[Execute] wait" << std::endl;
            condition.wait(lock);
        }

        //Jak stop jest true i kolejka jest pusta to zatrzymujemy wątki
        if (stop && tasks.empty()) {
            std::cout << "[Execute] stop and return" << std::endl;
            return;
        }

        //zdejmowanie pierwszego elementu z kolejki
        std::cout << "[Execute] pop" << std::endl;
//        condition.notify_one();
        Task task = tasks.front();
        tasks.pop();

        //odblokowanie mutexa przed wykonaniem zadania(tutaj funkcji) aby inne wątki mogły dodać zadania
        std::cout << "[Execute] unlock" << std::endl;
        lock.unlock();
        std::cout << "[Execute] execute" << std::endl;
        task.func(task.arg);
    }
}

void ThreadPool::logWithThreadIdAndTime(const std::string &where, const std::string &message) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto micro = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%F %T") << "." << std::setw(3) << std::setfill('0') << (millis % 1000);

    //casting thread id to int
    std::thread::id thread_id = std::this_thread::get_id();
    std::stringstream ssForId;
    ssForId << thread_id;
    unsigned long long id_as_ull;
    ssForId >> id_as_ull;
    std::cout << "[" + where + "]" +
                 ss.str() << "\t" << id_as_ull << "\t" << message << std::endl;
}
