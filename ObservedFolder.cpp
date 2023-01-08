//
// Created by bartl on 06.01.2023.
//

#include <future>
#include <iostream>
#include "ObservedFolder.h"

ObservedFolder::ObservedFolder(const std::filesystem::path &folderPath) :
        folderPath_(folderPath),
        lastModyfiedTime_(std::filesystem::last_write_time(folderPath_)) {

}

void ObservedFolder::registerObserver(Observer *observer) {
    this->observers_.push_back(observer);
}

void ObservedFolder::checkForChanges() {
    auto currentTimeInFolder = std::filesystem::last_write_time(folderPath_);
    //curr time is always bigger
    if (currentTimeInFolder > lastModyfiedTime_) {
        lastModyfiedTime_ = currentTimeInFolder;
        notifyObservers();
    }
}

void ObservedFolder::notifyObservers() {
    for (auto observer: observers_) {
        std::cout << "Notify observers. Run update asynchronously" << std::endl;
        auto updateTask = std::async(std::launch::async, &Observer::update, observer);
    }
}

std::filesystem::path ObservedFolder::getFolderPath() const {
    return folderPath_;
}

void ObservedFolder::autoCheckForChangesStart() {
    std::lock_guard<std::mutex> lock(autoCheckMutex_);
    if (!enableAutoSync_.load()) {
        enableAutoSync_.store(true);
        autoCheckThread_ = std::thread(&ObservedFolder::infiniteCheckForChanges, this);
    }
}

void ObservedFolder::autoCheckForChangesStop() {
    std::lock_guard<std::mutex> lock(autoCheckMutex_);
    enableAutoSync_.store(false);
    //check if thread is still running
    if (autoCheckThread_.joinable()) {
        std::cout << "enableAutoSync_: " << enableAutoSync_ << std::endl;
//        autoCheckThread_.join();
        ObservedFolder::autoCheckThread_.join();
        std::cout << "Auto sync just finished: " << std::endl << std::endl;
    }
}

ObservedFolder::~ObservedFolder() {
    autoCheckThread_.join();
}

void ObservedFolder::infiniteCheckForChanges() {
    while (enableAutoSync_.load()) {
        notifyObservers();
        std::cout << "Sleeping for 1 seconds" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

