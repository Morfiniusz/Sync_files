//
// Created by bartl on 06.01.2023.
//

#ifndef SYNC_FILES_OBSERVEDFOLDER_H
#define SYNC_FILES_OBSERVEDFOLDER_H


#include <list>
#include <filesystem>
#include "Observer.h"

class ObservedFolder {

public:
    ObservedFolder(const std::string &folderPath) ;
    //Add folders which observes this object/folder to the list of observers
    void registerObserver(Observer* observer);

    //Check if sth has changed in observed folder
    void checkForChanges();
    void notifyObservers();

private:
    std::list<Observer *> observers_;
    std::filesystem::path folder_;
    std::filesystem::file_time_type lastModyfiedTime_;
};


#endif //SYNC_FILES_OBSERVEDFOLDER_H
