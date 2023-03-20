#ifndef SCAN_FOLDER_H
#define SCAN_FOLDER_H

#include <vector>
#include <filesystem>


struct ScanItem {
    std::string fileName;
    std::filesystem::path filePath;
    std::filesystem::file_time_type modyficationTime;
    std::string md5Sum;
};

std::vector<ScanItem> scanFolder(std::filesystem::path dir);

#endif //SCAN_FOLDER_H
