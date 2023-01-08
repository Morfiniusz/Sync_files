#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <filesystem>
#include <atomic>
#include <stdio.h>
#include <future>

namespace fs = std::filesystem;
fs::file_time_type sourceTimePrevious;

std::atomic<bool> once = true;
std::atomic<bool> autoFolderSync{false};
std::atomic<bool> threadRun{true};

std::mutex mutexDiffrenceFun;

enum class MENU_OPTIONS {
    RUN_ONCE = 1,
    AUTO_SYNC_ON,
    AUTO_SYNC_OFF,
    END
};

///
/// \param source
/// \param destination
/// \param mutex
/// \param cv
/// \param isReady
/// @brief The function browses directories and synchronizes from source to destination folder
void folderSync(const fs::path &source, const fs::path &destination) {
    std::cout << "Folder sync begin" << std::endl;

    std::unique_lock<std::mutex> lock(mutexDiffrenceFun); //TODO: Make this function thread safe. Add lock_guard
//    cv.wait(lock, [&isReady] { return isReady; });

    //check once
    if (once) {
        sourceTimePrevious = fs::last_write_time(source);
        once = false;
    }
    auto sourceTime = fs::last_write_time(source);

    //check if the source folder_ has been modified and then copy the files to the destination folder_
    for (auto &entry: fs::recursive_directory_iterator(source)) {
        if (entry.is_directory()) {
            auto path = entry.path();
            auto relativePath = fs::relative(path, source);
            auto destinationPath = destination / relativePath;
            if (!fs::exists(destinationPath)) {
                fs::remove_all(destinationPath); //TODO: get rid of remove_all
                fs::copy(path, destinationPath, fs::copy_options::recursive);
            } else {
                if (sourceTime != sourceTimePrevious) {
                    std::cout << "Source folder_ has been modified" << std::endl;
                    std::cout << "Source time: " << sourceTime.time_since_epoch().count() << std::endl;
                    std::cout << "sourceTimePrevious time: " << sourceTimePrevious.time_since_epoch().count()
                              << std::endl;

                    fs::remove_all(destinationPath); //TODO: get rid of remove_all
                    fs::copy(path, destinationPath, fs::copy_options::recursive);

                    sourceTimePrevious = sourceTime;
                }
            }
        }
    }
    std::cout << "Folder sync end" << std::endl;
}

const fs::path currentPath = fs::current_path();
const fs::path source = currentPath.parent_path() / "Test/TestFolder/MasterFolder";
const fs::path destination = currentPath.parent_path() / "Test/TestFolder/DestinationFolder";

const int menuOptionExit = 4;

void displayMenu() {
    std::cout << "1. Run sync once" << std::endl;
    std::cout << "2. Start auto sync [100 ms]" << std::endl;
    std::cout << "3. Stop auto sync [100 ms]" << std::endl;
    std::cout << "4. End" << std::endl;
}

void runDiff() {
    while (threadRun) {
        if (autoFolderSync) {
            auto var = std::async(std::launch::async, folderSync, source, destination);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main() {
    std::cout << "Source: " << source << std::endl;
    std::cout << "Destination: " << destination << std::endl;

    std::mutex mutex;
    std::condition_variable cv;

    std::thread t1(runDiff);

    int option = 0;
    MENU_OPTIONS menuOptions = MENU_OPTIONS::END;
    while (option != menuOptionExit) {
        displayMenu();
        std::cin >> option;
        menuOptions = static_cast<MENU_OPTIONS>(option);
        switch (menuOptions) {
            case MENU_OPTIONS::RUN_ONCE: {
                auto var = std::async(std::launch::async, folderSync, source, destination);
                break;
            }

            case MENU_OPTIONS::AUTO_SYNC_ON: {
                autoFolderSync.store(true);
                break;
            }

            case MENU_OPTIONS::AUTO_SYNC_OFF: {
                autoFolderSync.store(false);
                break;
            }

            case MENU_OPTIONS::END: {
                //stop thread responsible for auto sync of folders
                threadRun.store(false);
                break;
            }


            default:
                std::cout << "Unimplemented function" << std::endl;
                break;
        }
    };

    t1.join();

    return 0;
}