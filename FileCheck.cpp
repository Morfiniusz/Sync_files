//
// Created by bartl on 25.02.2023.
//

#include <string>
#include <filesystem>
#include <fstream>
#include "FileCheck.h"

namespace fs = std::filesystem;

FileCheck::FileCheck(const fs::path &folderPath) {
    // Get the MD5 file path
    fs::path md5file = folderPath / "MD5_CHECKSUM";	

    // Check if the checksum file exists in the folder
    if (fs::exists(md5file)) {

        //Open the MD5 file
        std::ifstream md5fstream(md5file, std::ios::binary);
        std::string buffer;
        while (std::getline(md5fstream, buffer)) {
            //Split the string by ',' and store the first part into fileName
            std::string fileName = buffer.substr(0, buffer.find(','));
            //Store the second part into hashValue
            std::string hashValue = buffer.substr(buffer.find(',') + 1);

            // Add the file name and hash value to the map
            m_map[fileName] = hashValue;
        }
    }
}


