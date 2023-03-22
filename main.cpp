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
const std::filesystem::path mainFolderPath = currentPath.parent_path() / "CheckFolder";
const std::filesystem::path sourcePath = currentPath.parent_path() / "CheckFolder\\MasterFolder";
const std::filesystem::path destinationPath = currentPath.parent_path() / "CheckFolder\\DestinationFolder";
const std::filesystem::path destinationPath2 = currentPath.parent_path() / "CheckFolder\\DestinationFolder2";

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

int main() {
    FileCheck fileCheck;
    SyncDirectories sync;
    auto vec1 = scanFolder(sourcePath);
    auto vec2 = scanFolder(destinationPath);
    auto vec3 = scanFolder(destinationPath2);

#ifdef LOG_ENABLED
    std::cout << "vec.size: " << vec1.size() << std::endl;
    std::cout << "[main] Thread pool test" << std::endl;
#endif
    ThreadPool threadPool(4);
    // Enqueue tasks
    threadPool.enqueueTask([&sync](size_t idx) { sync.syncDirectories(idx); }, static_cast<size_t>(1));
    threadPool.enqueueTask([&sync](size_t idx) { sync.syncDirectories(idx); }, static_cast<size_t>(2));
    threadPool.enqueueTask([&sync](size_t idx) { sync.syncDirectories(idx); }, static_cast<size_t>(1));
    threadPool.threadLogger("main     ", "Time for execution!");

    return 0;
}
