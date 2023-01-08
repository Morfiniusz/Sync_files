//
// Created by bartl on 06.01.2023.
//

#ifndef SYNC_FILES_OBSERVEDFOLDER_H
#define SYNC_FILES_OBSERVEDFOLDER_H


#include <list>
#include <filesystem>
#include "Observer.h"
#include <thread>

class ObservedFolder {

public:
    ObservedFolder(const std::filesystem::path &folderPath);

    ~ObservedFolder();

    //Add folders which observes this object/folder to the list of observers
    void registerObserver(Observer *observer);

    //Check if sth has changed in observed folder
    void checkForChanges();

    void autoCheckForChangesStart();

    void autoCheckForChangesStop();

    void infiniteCheckForChanges();

    void notifyObservers();

    std::filesystem::path getFolderPath() const;

    std::thread autoCheckThread_;

private:
    std::list<Observer *> observers_;
    std::filesystem::path folderPath_;

    std::filesystem::file_time_type lastModyfiedTime_;
    std::atomic<bool> enableAutoSync_{false};
    std::mutex autoCheckMutex_;
};


#endif //SYNC_FILES_OBSERVEDFOLDER_H
