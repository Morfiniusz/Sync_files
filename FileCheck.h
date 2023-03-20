//
// Created by bartl on 25.02.2023.
//

#ifndef SYNC_FILES_FILECHECK_H
#define SYNC_FILES_FILECHECK_H

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <openssl/md5.h>

class FileCheck {
public:
    //add md5 file checking
    static std::string getMD5(const std::filesystem::path& filePath);

    private:
    std::filesystem::path folderPath;
};


#endif //SYNC_FILES_FILECHECK_H
