#include <algorithm>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include "ScanFolder.h"
#include "SyncDirectories.h"



void SyncDirectories::printVec(const std::vector<ScanItem>& vec) {
    for( auto& item : vec )
    {
        std::cout << "File name: " << item.fileName << '\n';
        std::cout << "File path: " << item.filePath << '\n';
        std::time_t cftime = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(std::filesystem::file_time_type::clock::time_point(item.modyficationTime)));
        std::stringstream ss;
        ss << std::put_time(std::localtime(&cftime), "%Y-%m-%d %X");
        std::cout << "File modification time: " << ss.str() << '\n';
        std::cout << "File md5: " << item.md5Sum << '\n';
        std::cout << "---" << '\n';
    }
}

void SyncDirectories::printPairs (std::vector<std::pair<size_t, std::vector<ScanItem>>> vec) {
    for(const auto& el : vec) {
        std::cout << "-----------------------------Index: " << el.first << " ------------------------------------\n";
        printVec(el.second);
    }
}

void SyncDirectories::printMap(const std::unordered_map<std::string, ScanItem>& mapOfItems) {
    for(const auto& [key,value] : mapOfItems) {
        std::cout << "Key: " << key << " Path: " << value.filePath.parent_path() << '\n';
    }
}

auto SyncDirectories::pathFinder(const std::filesystem::path& mainPath) {
    std::vector<std::pair<size_t,std::filesystem::path>> vecOfPaths;
    size_t idy {0};
    for (std::filesystem::path path : std::filesystem::directory_iterator(mainPath)) {
        vecOfPaths.emplace_back(std::make_pair(idy++, path));
    }
    vecOfPaths.shrink_to_fit();
    return vecOfPaths;
}



auto SyncDirectories::stateCreator(const std::vector<std::pair<size_t,std::filesystem::path>>& vecOfPaths) {
    std::vector<std::pair<size_t, std::vector<ScanItem>>> vecOfStates;
    size_t id {0};
    for (const auto& el: vecOfPaths) {
        vecOfStates.push_back(std::make_pair(id++, scanFolder(el.second)));
    }
    vecOfStates.shrink_to_fit();
    return vecOfStates;
}

// size_t SyncDirectories::getIdx() const {
//     return idx_;
// }

// void SyncDirectories::setIdx(size_t newIdx) {
//     idx_ = newIdx;
// }



void SyncDirectories::syncDirectories(const size_t& idx) {

    std::vector<std::pair<size_t, std::filesystem::path>> vecOfPaths = pathFinder(mainFolderPath);
    std::vector<std::pair<size_t, std::vector<ScanItem>>> vecOfStates = stateCreator(vecOfPaths);

    //printPairs(vecOfStates);

    const std::vector<ScanItem>& idxVec = vecOfStates.at(idx).second;
    const auto& idxVecPath = vecOfPaths.at(idx).second;
    std::unordered_map<std::string, ScanItem> mapOfItemsToCopy;

    for (const auto& [idxOfVec, vecOfItemsToCheck] : vecOfStates) {
        if(idxOfVec == idx ) continue;
        for(const auto& itemsToCheck : vecOfItemsToCheck) {
            const auto& [itemFileName, itemPath, itemModificationTime, itemMd5Sum] = itemsToCheck;
            for(auto it = idxVec.begin(); it != idxVec.end();++it) {
                if (vecOfItemsToCheck.size() == 0) {
                    std::filesystem::copy_file(it->filePath, vecOfPaths.at(idx).second);
                    std::cout << "Empty destination folder copying files from source directory";
                }
                if(it->fileName == itemFileName && it->md5Sum == itemMd5Sum) {
                    std::cout << "Source directory: " << it->md5Sum << " | Target directory: " << itemMd5Sum << " Target directory path: " << itemPath << '\n';
                    if(it->modyficationTime > itemModificationTime) {
                        std::filesystem::copy_file(it->filePath, itemPath, std::filesystem::copy_options::overwrite_existing);
                        std::cout << "Modified file " << itemFileName << " overwite by: " << it->fileName << " from " << it->filePath.parent_path() << '\n';
                    }
                    if(it->modyficationTime < itemModificationTime) {
                        std::cout << "Skipping file " << itemFileName << " in " << itemPath << " newer file than in " << it->fileName << '\n';
                        // std::filesystem::copy_file(itemFileName, it->filePath, std::filesystem::copy_options::overwrite_existing);
                        // std::cout << "Update in main directory " << it->fileName << " older than in " << itemPath << '\n';
                    }
                } else if (it->fileName == itemFileName && it->md5Sum != itemMd5Sum) {
                     if(it->modyficationTime > itemModificationTime) {
                        std::filesystem::copy_file(it->fileName, itemPath, std::filesystem::copy_options::overwrite_existing);
                        std::cout << "Modified file " << itemFileName << "Path: " << itemPath << " overwite by: " << it->fileName << " from " << it->filePath.parent_path() << '\n';
                    }
                    if(it->modyficationTime < itemModificationTime) {
                        std::cout << "Skipping file " << itemFileName << " in " << itemPath << " newer file than in " << it->filePath << '\n';
                        std::filesystem::copy(itemPath, it->filePath, std::filesystem::copy_options::overwrite_existing);
                        //std::filesystem::copy_file(itemFileName, it->filePath, std::filesystem::copy_options::overwrite_existing);
                        std::cout << "Update in main directory " << it->fileName << " older than in " << itemPath << '\n';
                    }
                }
            }
        }
    }
}

// for (const auto& [idxOfVec, vecOfItemsToCheck] : vecOfStates) {
//         if(idxOfVec == idx ) continue;
//         for(const auto& itemsToCheck : vecOfItemsToCheck) {
//             for (const auto& itemIdx : idxVec) {
//                 if(itemIdx.fileName == itemsToCheck.fileName && itemIdx.md5Sum == itemsToCheck.md5Sum) {
//                     if(itemIdx.modyficationTime > itemsToCheck.modyficationTime) {
//                         std::filesystem::copy_file(itemIdx.filePath, itemsToCheck.filePath, std::filesystem::copy_options::overwrite_existing);
//                         std::cout << "Modified file " << itemsToCheck.fileName << "overwite by: " << itemIdx.fileName << " from " << itemIdx.filePath.parent_path() << '\n';
//                     }
//                     else if (itemIdx.modyficationTime <= itemsToCheck.modyficationTime) {
//                         std::cout << "Skipping file " << itemsToCheck.fileName << "Newer file than in " << itemIdx.filePath << '\n';
//                     }
//                 }
//             }
//         }
//     }

            // for (const auto& itemIdx : idxVec) {
                
            //     if(itemIdx.fileName == itemsToCheck.fileName && itemIdx.md5Sum == itemsToCheck.md5Sum) {
            //         if(itemIdx.modyficationTime > itemsToCheck.modyficationTime) {
            //             std::filesystem::copy_file(itemIdx.filePath, itemsToCheck.filePath, std::filesystem::copy_options::overwrite_existing);
            //             std::cout << "Modified file " << itemsToCheck.fileName << "overwite by: " << itemIdx.fileName << " from " << itemIdx.filePath.parent_path() << '\n';
            //         }
            //         else if (itemIdx.modyficationTime <= itemsToCheck.modyficationTime) {
            //             std::cout << "Skipping file " << itemsToCheck.fileName << "Newer file than in " << itemIdx.filePath << '\n';
            //         }
            //     }
            // }




