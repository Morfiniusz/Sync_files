#include <algorithm>
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

    printPairs(vecOfStates);

    const std::vector<ScanItem>& idxVec = vecOfStates.at(idx).second;
    const auto& idxVecPath = vecOfPaths.at(idx).second;
    std::unordered_map<std::string, ScanItem> mapOfItemsToCopy;

    for (const auto& [idxOfVec, vecOfItemsToCheck] : vecOfStates) {
        if (idxOfVec == idx) {
            continue; // skip target directory
        }
        for (const auto& item : vecOfItemsToCheck) {
            const auto& [itemName, itemPath, itemModTime, itemMd5Sum] = item;
            auto it = idxVec.begin();
            while (it != idxVec.end()) {
                if (it->fileName == itemName && it->modyficationTime >= itemModTime && it->md5Sum == itemMd5Sum) {
                    std::cout << "Skipping file " << it->fileName << " in " << it->filePath << " " << "compared to " << " it's up-to-date\n";
                    break;
                }
                ++it;
            }
            if (it == idxVec.end()) {
                // File not found in target directory, add to copy map
                auto existingIt = mapOfItemsToCopy.find(itemName);
                if (existingIt == mapOfItemsToCopy.end() || (existingIt->second.modyficationTime < itemModTime && existingIt->second.md5Sum != itemMd5Sum)) {
                    mapOfItemsToCopy[itemName] = item;
                }
            }
        }
    }

    for (const auto& idItem : idxVec) {
        bool found = false;
        for (const auto& [idOfVec, vecOfItems] : vecOfStates) {
            if (idOfVec == idx) continue;
            if (std::ranges::any_of(vecOfItems, [&idItem](const auto& lhs) {return lhs.fileName == idItem.fileName;})) {
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "Deleted file " << idItem.fileName << " from " << idItem.filePath.parent_path() << '\n';
            std::filesystem::remove(idItem.filePath);
        }
    }



    for (const auto& [name, item] : mapOfItemsToCopy) {
        const auto& [itemName, itemPath, itemModTime, itemMd5Sum] = item;
        const auto& idxFilePath = idxVecPath / itemName;
        std::filesystem::copy_file(itemPath, idxFilePath, std::filesystem::copy_options::overwrite_existing);
        std::cout << "Copied file " << itemName << " from " << itemPath << " to " << idxFilePath << '\n';
    }

    vecOfStates = stateCreator(vecOfPaths);
    printPairs(vecOfStates);
}



