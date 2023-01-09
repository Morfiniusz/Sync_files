//
// Created by bartl on 09.01.2023.
//

#ifndef SYNC_FILES_THREADTIMER_H
#define SYNC_FILES_THREADTIMER_H


#include <functional>
#include <atomic>
#include <thread>

template<typename T>
class ThreadTimer {
public:
    ThreadTimer(std::function<const T&> fun);
    ~ThreadTimer();

    void runDiff();

    void start();

    void stop();

private:
    std::atomic<bool> synchronization{false};
    std::atomic<bool> threadRun{true};
    std::function<T> fun_;
    std::thread worker;
};

template<typename T>
void ThreadTimer<T>::runDiff() {
    while (threadRun) {
        if (synchronization) {
            fun_();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

template<typename T>
ThreadTimer<T>::ThreadTimer(std::function<const T&> fun) : fun_(fun) {

}


#endif //SYNC_FILES_THREADTIMER_H
