//
// Created by bartl on 10.01.2023.
//

#include "SyncFolder.h"


SyncFolder::SyncFolder(const std::filesystem::path &path) :
        ObserverFolder(path),
        ObservedFolder(path) {

}
