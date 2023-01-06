//
// Created by bartl on 06.01.2023.
//

#include "ObservedFolder.h"

ObservedFolder::ObservedFolder(const std::string &folderPath) : folder_(folderPath),
                                                                lastModyfiedTime_(
                                                                        std::filesystem::last_write_time(folder_)) {}

void ObservedFolder::registerObserver(Observer *observer) {
    this->observers_.push_back(observer);
}

void ObservedFolder::checkForChanges() {
    auto currentTimeInFolder = std::filesystem::last_write_time(folder_);
    //curr time is always bigger
    if (currentTimeInFolder > lastModyfiedTime_) {
        lastModyfiedTime_ = currentTimeInFolder;
        notifyObservers();
    }
}

void ObservedFolder::notifyObservers() {
    for (auto observer: observers_) {
        observer->update();
    }
}

