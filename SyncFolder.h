//
// Created by bartl on 10.01.2023.
//

#ifndef SYNC_FILES_SYNCFOLDER_H
#define SYNC_FILES_SYNCFOLDER_H


#include "ObserverFolder.h"
#include "ObservedFolder.h"

class SyncFolder : public ObserverFolder, public ObservedFolder {
public:
    SyncFolder(const std::filesystem::path &path);

    //virtual destructor
    virtual ~SyncFolder() = default;

    using ObservedFolder::registerObserver;
    using ObservedFolder::checkForChanges;
};


#endif //SYNC_FILES_SYNCFOLDER_H
