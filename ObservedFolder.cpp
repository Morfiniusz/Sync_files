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
    std::cout << "Checking for changes in folder: " << folderPath_ << std::endl;
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

ObservedFolder::~ObservedFolder() {
    if (autoCheckThread_.joinable()) {
        autoCheckThread_.join();
    }
}



