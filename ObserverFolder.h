//
// Created by bartl on 06.01.2023.
//

#ifndef SYNC_FILES_OBSERVERFOLDER_H
#define SYNC_FILES_OBSERVERFOLDER_H


#include <string>
#include <filesystem>
#include "Observer.h"
#include "ObservedFolder.h"

class ObserverFolder : public Observer {
public:
    ObserverFolder(const std::filesystem::path &folderPath, ObservedFolder *observedFolder);

    /// \brief Do folder synchronization here
    void update() override;

private:
    ObservedFolder *observedFolder_;
    std::filesystem::path folderPath_;

    std::mutex mutex_;

    bool once{true};
    std::filesystem::file_time_type sourceTimePrevious{std::filesystem::file_time_type::min()};
};


#endif //SYNC_FILES_OBSERVERFOLDER_H
