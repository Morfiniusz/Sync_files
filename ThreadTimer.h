//
// Created by bartl on 09.01.2023.
//

#ifndef SYNC_FILES_THREADTIMER_H
#define SYNC_FILES_THREADTIMER_H


#include <functional>
#include <atomic>
#include <thread>
#include "ObservedFolder.h"
#include "ObserverFolder.h"

class ThreadTimer {
public:

    ThreadTimer(std::function<void()> &&f);

    ~ThreadTimer();

    void runDiff();

    void start();

    void stop();

    void stopThread();

private:
    std::function<void()> fun_;
    std::thread worker;
    std::atomic<bool> enableThreadFun{false};
    std::atomic<bool> enableThreadRun{true};
};

#endif //SYNC_FILES_THREADTIMER_H
