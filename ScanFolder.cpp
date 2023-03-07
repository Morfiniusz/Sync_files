#include "ScanFolder.h"
#include "FileCheck.h"
#include <iostream>


std::vector<ScanItem> scanFolder(std::filesystem::path dir)
{
    std::vector<ScanItem> result;
    for (auto &entry: std::filesystem::recursive_directory_iterator(dir)) {
        if(entry.is_regular_file())
        {
            ScanItem item;
            item.filePath = entry.path();
            item.modyficationTime = std::filesystem::last_write_time(entry);
            item.md5Sum = FileCheck::getMD5(item.filePath);

            result.emplace_back(item);
        }
    }

    return result;
}
