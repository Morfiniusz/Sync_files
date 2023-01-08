//
// Created by bartl on 06.01.2023.
//

#include <filesystem>
#include <iostream>
#include "ObserverFolder.h"


ObserverFolder::ObserverFolder(const std::filesystem::path &folderPath, ObservedFolder *observedFolder) :
        observedFolder_(observedFolder),
        folderPath_(folderPath) {
}

void ObserverFolder::update() {
    //mutex
    std::lock_guard<std::mutex> lock(mutex_);

    std::cout << "Folder sync begin in OBSERVER!!! for folder: \n" << folderPath_ << std::endl;

    //TODO: make it async for every 'entry'
    for (auto &entry: std::filesystem::recursive_directory_iterator(observedFolder_->getFolderPath())) {
        std::cout << "entry.path()" << entry.path() << std::endl;

        //Works only for folders
        if (entry.is_directory()) {
            auto path = entry.path();
            auto relativePath = std::filesystem::relative(path, observedFolder_->getFolderPath());
            auto destinationPathForCheck = this->folderPath_ / relativePath;
            if (!std::filesystem::exists(destinationPathForCheck)) {
                std::cout << "Destination folderPath_ doesn't exist" << std::endl;
                std::filesystem::remove_all(destinationPathForCheck); //TODO: get rid of remove_all
                std::filesystem::copy(path, destinationPathForCheck, std::filesystem::copy_options::recursive);
            } else {
                std::cout << "Source folderPath_ has been modified" << std::endl;
                std::filesystem::remove_all(destinationPathForCheck); //TODO: get rid of remove_all
                std::filesystem::copy(path, destinationPathForCheck, std::filesystem::copy_options::recursive);
            }
        }
    }
    std::cout << "Folder sync ended in OBSERVER!!!for folder: \n" << folderPath_ << std::endl << std::endl;
}
