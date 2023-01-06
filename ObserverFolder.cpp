//
// Created by bartl on 06.01.2023.
//

#include "ObserverFolder.h"


ObserverFolder::ObserverFolder(const std::string &folderPath,
                               ObservedFolder *observedFolder) : observedFolder_(observedFolder),
                                                                 folder_(folderPath) {

}

void ObserverFolder::update() {
}
