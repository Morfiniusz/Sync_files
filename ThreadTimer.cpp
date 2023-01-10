//
// Created by bartl on 09.01.2023.
//

#include "ThreadTimer.h"

ThreadTimer::ThreadTimer(std::function<void()> &&f) : fun_(std::move(f)) {
    worker = std::thread(&ThreadTimer::runDiff, this);
}

ThreadTimer::~ThreadTimer() {
    if (worker.joinable())
        worker.join();
}

void ThreadTimer::runDiff() {
    while (enableThreadRun) {
        if (enableThreadFun) {
            fun_();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void ThreadTimer::start() {
    enableThreadFun.store(true);
}

void ThreadTimer::stop() {
    enableThreadFun.store(false);
}

void ThreadTimer::stopThread() {
    enableThreadRun.store(false);
}



