//
// Created by on 06.01.2023.
//

#include <filesystem>
#include <iostream>
#include <mutex>
#include "ObserverFolder.h"


ObserverFolder::ObserverFolder(const std::filesystem::path &folderPath) :
        folderPath_(folderPath) {
}

void ObserverFolder::update(std::filesystem::path &observedPath) {

    std::cout << "Folder sync begin IN folder: \t" << observedPath.filename() << std::endl;
    std::cout << "Folder sync begin FOR folder: \t" << folderPath_.filename() << std::endl;
    //TODO: make it async for every 'entry'
    for (auto &entry: std::filesystem::recursive_directory_iterator(observedPath)) {
//        std::cout << "entry.path()" << entry.path() << std::endl;

        //Works only for folders
        if (entry.is_directory()) {
            auto path = entry.path();
            auto relativePath = std::filesystem::relative(path, observedPath);
            auto destinationPathForCheck = this->folderPath_ / relativePath;
            if (!std::filesystem::exists(destinationPathForCheck)) {
                std::cout << "Destination folderPath_ doesn't exist" << std::endl;
                std::filesystem::remove(destinationPathForCheck); //TODO: get rid of remove_all
                std::filesystem::copy(path, destinationPathForCheck, std::filesystem::copy_options::recursive);
            } else {
                std::cout << "Source folderPath_ has been modified" << std::endl;
                std::filesystem::remove(destinationPathForCheck); //TODO: get rid of remove_all
                std::filesystem::copy(path, destinationPathForCheck, std::filesystem::copy_options::recursive);
            }
        } else {
            auto path = entry.path();
            auto relativePath = std::filesystem::relative(path, observedPath);
            auto destinationPathForCheck = this->folderPath_ / relativePath;
            if (!std::filesystem::exists(destinationPathForCheck)) {
                std::cout << "Destination file doesn't exist" << std::endl;
                std::filesystem::remove(destinationPathForCheck);
                std::filesystem::copy_file(path, destinationPathForCheck);
            } else {
                std::cout << "Source file has been modified" << std::endl;
                std::filesystem::remove(destinationPathForCheck);
                std::filesystem::copy_file(path, destinationPathForCheck);
            }
        }
    }
    std::cout << "Folder sync ended in OBSERVER!!! \n" << std::endl << std::endl;
}
