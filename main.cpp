#include <iostream>
#include <filesystem>
#include <future>
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


//TODO: Thread pool - Bart
//TODO: State compare - Lukasz
//TODO: Scan folder - Marci

//TODO: Action decision maker?
int main() {
    FileCheck fileCheck;
    // std::filesystem::path md5Path("D:/CPP/AdvancedCpp/Projekt1/Sync_files/Test/TestFolder/MasterFolder/md5.txt");
    std::filesystem::path md5Path("../Test/TestFolder/MasterFolder/md5.txt");
    std::cout << "MD5: " << fileCheck.getMD5(md5Path) << std::endl;

    auto vec = scanFolder(sourcePath);

    std::cout << "vec.size: " << vec.size() << std::endl;


    for( auto& item : vec )
    {
        std::cout << "path: " << item.filePath << std::endl;

        // std::time_t cftime = decltype(item.modyficationTime)::clock::to_time_t(item.modyficationTime);
        std::time_t cftime = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(std::filesystem::file_time_type::clock::time_point(item.modyficationTime)));
        std::stringstream ss;
        ss << std::put_time(std::localtime(&cftime), "%Y-%m-%d %X");
        std::cout << "modyficationTime: " << ss.str() << std::endl;

        // std::cout << "modyficationTime: " << decltype(item.modyficationTime)::clock::to_time_t(item.modyficationTime) << std::endl;
        std::cout << "md5: " << item.md5Sum << std::endl;
        std::cout << "---" << std::endl;
    }


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