#include <algorithm>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <future>
#include <ranges>
#include <vector>
#include "ObservedFolder.h"
#include "ObserverFolder.h"
#include "ThreadTimer.h"
#include "SyncFolder.h"
#include "FileCheck.h"
#include "ScanFolder.h"
#include "SyncDirectories.h"

#include "ThreadPool.h"

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

void sampleTask(const std::string &message) {
    std::cout << "\n WYKKONANIE ZADANIA: " << message << std::endl;
}

// void printVec(const std::vector<ScanItem>& vec) {
//     for( auto& item : vec )
//     {
//         std::cout << "File name: " << item.fileName << std::endl;
//         std::cout << "File path: " << item.filePath << std::endl;

//         // std::time_t cftime = decltype(item.modyficationTime)::clock::to_time_t(item.modyficationTime);
//         std::time_t cftime = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(std::filesystem::file_time_type::clock::time_point(item.modyficationTime)));
//         std::stringstream ss;
//         ss << std::put_time(std::localtime(&cftime), "%Y-%m-%d %X");
//         std::cout << "File modification time: " << ss.str() << std::endl;
//         // std::cout << "modyficationTime: " << decltype(item.modyficationTime)::clock::to_time_t(item.modyficationTime) << std::endl;
//         std::cout << "File md5: " << item.md5Sum << std::endl;
//         std::cout << "---" << std::endl;
//     }
// }

int main() {
    FileCheck fileCheck;
    SyncDirectories sync;
    auto vec1 = scanFolder(sourcePath);
    auto vec2 = scanFolder(destinationPath);
    auto vec3 = scanFolder(destinationPath2);

    std::vector<std::filesystem::path> vecOfPaths;
    std::cout << "vec.size: " << vec1.size() << std::endl;
    sync.syncDirectories(1);

#ifdef LOG_ENABLED
    std::cout << "[main] Thread pool test" << std::endl;
#endif
    int threadNumbers = 4;
    ThreadPool threadPool(threadNumbers);
    // Zakolejkuj zadania
    for (int i = 0; i <= threadNumbers; ++i) {
        std::string message = "ZADANIE " + std::to_string(i) + "\n";
        threadPool.threadLogger("main     ", "Dodaj zadanie: " + std::to_string(i));
        threadPool.enqueueTask(sampleTask, message);
    }
    threadPool.threadLogger("main     ", "Time for execution!");
    threadPool.executeTasks();

//     ErrorCode variable = stateCompare(vec, vec2);
//     switch(variable) {
//         case ErrorCode::File_Exist_replace: {
//             std::cout << "sth2\n";
//             break;
//         }
//         case ErrorCode::File_Exist_dont_replace: {
//             std::cout << "sth3\n";
//             break;
//         }
//     }
//
//    {
//        for (auto folder: syncFolders) {
//            for (auto otherFolder: syncFolders) {
//                if (folder != otherFolder) {
//                    folder->registerObserver(otherFolder.get());
//                }
//            }
//        }
//
//        for (auto &syncFolder: syncFolders) {
//            threadTimers.emplace_back(
//                    std::make_shared<ThreadTimer>([&syncFolder]() { syncFolder->checkForChanges(); }));
//        }
//
//        mainMenu(threadTimers);
//    }
//     {
//         for (auto folder: syncFolders) {
//             for (auto otherFolder: syncFolders) {
//                 if (folder != otherFolder) {
//                     folder->registerObserver(otherFolder.get());
//                 }
//             }
//         }
//
//         for (auto &syncFolder: syncFolders) {
//             threadTimers.emplace_back(
//                     std::make_shared<ThreadTimer>([&syncFolder]() { syncFolder->checkForChanges(); }));
//         }
//
//         mainMenu(threadTimers);
//     }
    return 0;
}
