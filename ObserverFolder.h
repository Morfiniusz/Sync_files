//
// Created by bartl on 06.01.2023.
//

#ifndef SYNC_FILES_OBSERVERFOLDER_H
#define SYNC_FILES_OBSERVERFOLDER_H


#include <string>
#include "Observer.h"
#include "ObservedFolder.h"

class ObserverFolder : public Observer {
public:
    ObserverFolder(const std::string &folderPath, ObservedFolder *observedFolder);

    /// \brief Do folder synchronization here
    void update() override;

private:
    ObservedFolder *observedFolder_;
    std::filesystem::path folder_;

};


#endif //SYNC_FILES_OBSERVERFOLDER_H
