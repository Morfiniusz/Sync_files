#include "ThreadPool.h"

ThreadPool::ThreadPool(int numThreads) : stop(false) {
    std::cout << "ThreadPool constructor" << std::endl;
    for (int i = 0; i < numThreads; i++) {
        std::cout << "Create thread" << std::endl;
        threads.emplace_back([this] {
            std::cout << "Thread start" << std::endl;
            while (true) {
                std::cout<<"[wait] Thread while"<<std::endl;
                Task task;
                { // Zablokowanie sekcji krytycznej
                    unique_lock<mutex> lock(queue_mutex);
                    std::cout<<"[wait] Thread unique_lock"<<std::endl;

                    ///wait - oczekuje na sygnał od zmiennej condition
                    ///lambda - warunek sprawdzany przed każdym wybudzeniem
                    ///jeśli warunek jest spełniony to wychodzi z wait
                    ///jeśli warunek nie jest spełniony to odblokowuje mutex i czeka na sygnał
                    std::cout<<"[wait] Thread wait"<<std::endl;
                    condition.wait(lock, [this] { return stop || !tasks.empty(); });

                    std::cout<<"[wait] Thread wait end"<<std::endl;
                    if (stop && tasks.empty()) {
                        return;
                    }

                    std::cout<<"Thread move"<<std::endl;
                    //zdejmowania z kolejki i przypisywania do zmiennej task
                    task = std::move(tasks.front());

                    std::cout<<"Thread pop"<<std::endl;
                    //usuwanie z kolejki
                    tasks.pop();

                } // Odblokowanie sekcji krytycznej
                std::cout<<"Thread execute"<<std::endl;
                task.func(task.arg);
            }
        });
    }
}


void ThreadPool::enqueueTask(function<void(string)> func, string arg) {
    { // Zablokowanie sekcji krytycznej
        ///unique lock - blokuje mutex i odblokowuje go w destruktorze
        ///jest lepszy od lock_guard bo pozwala na odblokowanie mutexa w dowolnym miejscu
        ///queue_mutex, aby tylko jeden wątek mógł uzyskać dostęp do kolejki zadań.
        unique_lock<mutex> lock(queue_mutex);
        tasks.emplace(Task{func, arg});
    } // Odblokowanie sekcji krytycznej

    ///powiadomienie o dodaniu zadania do kolejki i odblokowanie mutexa i wybudzenie wątku
//    condition.notify_one();
}

ThreadPool::~ThreadPool() {

}

void ThreadPool::executeTasks() {
    while (true) {
        std::cout<<"[Execute] lock"<<std::endl;
        unique_lock<mutex> lock(queue_mutex);

        std::cout<<"[Execute] wait"<<std::endl;
        condition.notify_one();

        while (!stop && tasks.empty()) {
            //Czekanie na zadanie do wykonania i odblokowanie
            condition.wait(lock);
        }

        //Jak stop jest true i kolejka jest pusta to zatrzymujemy wątki
        if (stop && tasks.empty()) {
            return;
        }

        //zdejmowanie pierwszego elementu z kolejki
//        condition.notify_one();
        Task task = tasks.front();
        tasks.pop();

        //odblokowanie mutexa przed wykonaniem zadania(tutaj funkcji) aby inne wątki mogły dodać zadania
        lock.unlock();
        task.func(task.arg);
    }
}

