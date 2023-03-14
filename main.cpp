#include <algorithm>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <future>
#include <ranges>
#include <utility>
#include <unordered_map>
#include <vector>
#include "ObservedFolder.h"
#include "ObserverFolder.h"
#include "ThreadTimer.h"
#include "SyncFolder.h"
#include "FileCheck.h"
#include "ScanFolder.h"


std::atomic<bool> once = true;
std::atomic<bool> autoFolderSync{false};
std::atomic<bool> threadRun{true};

std::mutex mutexDiffrenceFun;

enum class MENU_OPTIONS {
    SYNC_ONCE = 1,
    AUTO_SYNC_ON,
    AUTO_SYNC_OFF,
    END
};


const std::filesystem::path currentPath = std::filesystem::current_path();
const std::filesystem::path mainFolderPath = currentPath.parent_path() / "Test/TestFolder";
const std::filesystem::path sourcePath = currentPath.parent_path() / "Test/TestFolder/MasterFolder";
const std::filesystem::path destinationPath = currentPath.parent_path() / "Test/TestFolder/DestinationFolder";
const std::filesystem::path destinationPath2 = currentPath.parent_path() / "Test/TestFolder/DestinationFolder2";

ObservedFolder observedFolder(sourcePath);
ObserverFolder observerFolder(destinationPath);
ObserverFolder observerFolder2(destinationPath2);

auto syncFolder1 = std::make_shared<SyncFolder>(sourcePath);
auto syncFolder2 = std::make_shared<SyncFolder>(destinationPath);
auto syncFolder3 = std::make_shared<SyncFolder>(destinationPath2);

std::vector<std::shared_ptr<SyncFolder>> syncFolders{syncFolder1, syncFolder2, syncFolder3};
std::vector<std::shared_ptr<ThreadTimer>> threadTimers;

void displayMenu() {
    std::cout << "1. Run sync once" << std::endl;
    std::cout << "2. Start auto sync [100 ms]" << std::endl;
    std::cout << "3. Stop auto sync [100 ms]" << std::endl;
    std::cout << "4. End" << std::endl;
}

void mainMenu(std::vector<std::shared_ptr<ThreadTimer>> threadTimersParam) {
    int option = 0;
    MENU_OPTIONS menuOptions{};
    while (menuOptions != MENU_OPTIONS::END) {
        displayMenu();
        //cast cin to MENU_OPTION
        std::cin >> option;
        menuOptions = static_cast<MENU_OPTIONS>(option);
        switch (menuOptions) {
            case MENU_OPTIONS::SYNC_ONCE: {
                syncFolder1->notifyObservers();
                syncFolder2->notifyObservers();
                syncFolder3->notifyObservers();
                break;
            }

            case MENU_OPTIONS::AUTO_SYNC_ON: {
                for (auto &threadTimer: threadTimersParam) {
                    threadTimer->startThreadFun();
                }
                break;
            }

            case MENU_OPTIONS::AUTO_SYNC_OFF: {
                for (auto &threadTimer: threadTimersParam) {
                    threadTimer->stopThreadFun();
                }
                break;
            }

            case MENU_OPTIONS::END: {
                for (auto &threadTimer: threadTimersParam) {
                    threadTimer->stopThread();
                }
                std::cout << "End" << std::endl;
                break;
            }

            default:
                std::cout << "Unimplemented function" << std::endl;
                threadRun.store(false);
                break;
        }
    }
}

void runDiff() {
    while (threadRun) {
        if (autoFolderSync) {
            syncFolder1->checkForChanges();
            syncFolder2->checkForChanges();
            syncFolder3->checkForChanges();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void printVec(const std::vector<ScanItem>& vec) {
    for( auto& item : vec )
    {
        std::cout << "File name: " << item.fileName << std::endl;
        std::cout << "File path: " << item.filePath << std::endl;

        // std::time_t cftime = decltype(item.modyficationTime)::clock::to_time_t(item.modyficationTime);
        std::time_t cftime = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(std::filesystem::file_time_type::clock::time_point(item.modyficationTime)));
        std::stringstream ss;
        ss << std::put_time(std::localtime(&cftime), "%Y-%m-%d %X");
        std::cout << "File modification time: " << ss.str() << std::endl;
        // std::cout << "modyficationTime: " << decltype(item.modyficationTime)::clock::to_time_t(item.modyficationTime) << std::endl;
        std::cout << "File md5: " << item.md5Sum << std::endl;
        std::cout << "---" << std::endl;
    }
}

void printMap(const std::unordered_map<std::string, ScanItem>& mapOfItems) {
    for(const auto& [key,value] : mapOfItems) {
        std::cout << "Key: " << key << " Path: " << value.filePath.parent_path() << '\n';
    }
}

void printTime(std::filesystem::file_time_type& time) {
    std::time_t cftime = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(std::filesystem::file_time_type::clock::time_point(time)));
    std::stringstream ss;
    ss << std::put_time(std::localtime(&cftime), "%Y-%m-%d %X");
}

void printPairs (std::vector<std::pair<size_t, std::vector<ScanItem>>> vec) {
    for(const auto& el : vec) {
        std::cout << "-----------------------------Index: " << el.first << " ------------------------------------\n";
        printVec(el.second);
    }
}

void printPaths(const std::vector<std::filesystem::path>& vec) {
    for (const auto& el : vec) {
        std::cout << " path: " << el << '\n';
    }
    std::cout << '\n';
}


void printNameofItem(std::vector<std::vector<ScanItem>> vec) {
    for (const auto& el : vec) {
        for (const auto& el_name : el) {
            std::cout << "File name: " << el_name.fileName << '\n';
        }
    }
}

auto pathFinder(const std::filesystem::path& mainPath) {
    std::vector<std::pair<size_t,std::filesystem::path>> vecOfPaths;
    size_t idx {0};
    for (std::filesystem::path path : std::filesystem::directory_iterator(mainPath)) {
        vecOfPaths.emplace_back(std::make_pair(idx++, path));
    }
    vecOfPaths.shrink_to_fit();
    return vecOfPaths;
}



auto stateCreator(const std::vector<std::pair<size_t,std::filesystem::path>>& vecOfPaths) {
    std::vector<std::pair<size_t, std::vector<ScanItem>>> vecOfStates;
    size_t idy {0};
    for (const auto& el: vecOfPaths) {
        vecOfStates.push_back(std::make_pair(idy++, scanFolder(el.second)));
    }
    vecOfStates.shrink_to_fit();
    return vecOfStates;
}



void syncDirectories(const size_t& idx) {

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
                if (it->fileName == itemName && it->modyficationTime >= itemModTime) {
                    std::cout << "Skipping file " << it->fileName << " in " << it->filePath << " - it's up-to-date\n";
                    break;
                }
                ++it;
            }
            if (it == idxVec.end()) {
                // File not found in target directory, add to copy map
                auto existingIt = mapOfItemsToCopy.find(itemName);
                if (existingIt == mapOfItemsToCopy.end() || 
                    (existingIt->second.modyficationTime < itemModTime && existingIt->second.md5Sum != itemMd5Sum)) {
                    mapOfItemsToCopy[itemName] = item;
                }
            }
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


//TODO: Thread pool - Bart
//TODO: State compare - Lukasz
//TODO: Scan folder - Marci

//TODO: Action decision maker?
int main() {
    FileCheck fileCheck;
    // std::filesystem::path md5Path("D:/CPP/AdvancedCpp/Projekt1/Sync_files/Test/TestFolder/MasterFolder/md5.txt");
    //std::filesystem::path md5Path("../Test/TestFolder/MasterFolder/md5.txt");
    //std::cout << "MD5: " << fileCheck.getMD5(md5Path) << std::endl;

    auto vec1 = scanFolder(sourcePath);
    auto vec2 = scanFolder(destinationPath);
    auto vec3 = scanFolder(destinationPath2);
    //size_t index {0};
    std::vector<std::filesystem::path> vecOfPaths;

    std::cout << "vec.size: " << vec1.size() << std::endl;
    // printVec(vec1);
    // printVec(vec2);
    //stateCompare(vec1, vec2);

    //vecOfPaths = pathFinder(mainFolderPath);
    //stateCreator(vecOfPaths);
    //printPairs(stateCreator(vecOfPaths));
    syncDirectories(1);

    
    // std::vector<std::vector<ScanItem>> vec = stateCompare(idx, vecOfPaths);

    // for (const auto & el: vec) {
    //     printVec(el);
    // }



    // ErrorCode variable = stateCompare(vec, vec2);
    // switch(variable) {
    //     case ErrorCode::File_Exist_replace: {
    //         std::cout << "sth2\n";
    //         break;
    //     }
    //     case ErrorCode::File_Exist_dont_replace: {
    //         std::cout << "sth3\n";
    //         break;
    //     }
    // }
    // {
    //     for (auto folder: syncFolders) {
    //         for (auto otherFolder: syncFolders) {
    //             if (folder != otherFolder) {
    //                 folder->registerObserver(otherFolder.get());
    //             }
    //         }
    //     }

    //     for (auto &syncFolder: syncFolders) {
    //         threadTimers.emplace_back(
    //                 std::make_shared<ThreadTimer>([&syncFolder]() { syncFolder->checkForChanges(); }));
    //     }

    //     mainMenu(threadTimers);
    // }

    return 0;
}