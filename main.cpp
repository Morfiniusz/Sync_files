#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;
fs::file_time_type sourceTimePrevious;

std::atomic<bool> once = true;


///
/// \param source
/// \param destination
/// \param mutex
/// \param cv
/// \param isReady
/// @brief The function browses directories and synchronizes from source to destination folder
void addDiffrenceFileToTemp(const fs::path &source,
                            const fs::path &destination,
                            std::mutex &mutex,
                            std::condition_variable &cv,
                            bool isReady) {

    std::unique_lock<std::mutex> lock(mutex); //TODO: Make this function thread safe. Add lock_guard
    cv.wait(lock, [&isReady] { return isReady; });




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
}

const fs::path currentPath = fs::current_path();
const fs::path source = currentPath.parent_path() / "Test\\TestFolder\\MasterFolder";
const fs::path destination = currentPath.parent_path() / "Test\\TestFolder\\DestinationFolder";

int main() {
    std::cout << "Source: " << source << std::endl;
    std::cout << "Destination: " << destination << std::endl;

    std::mutex mutex;
    std::condition_variable cv;

    do {
        //one way synnchronization from source to destination
        addDiffrenceFileToTemp(source, destination, mutex, cv, true);
        std::cout << "Sleeping..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    } while (true);

    return 0;
}