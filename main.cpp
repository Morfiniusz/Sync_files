#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;
fs::file_time_type sourceTimePrevious;

bool once = true;


///
/// \param source
/// \param destination
/// \param mutex
/// \param cv
/// \param isReady
/// @brief The function browses directories and synchronizes between
/// the source folder and the specified folder
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

    //Check recursively if the source and destination folders have the same number of subfolders
    //Check modification time of all subfolder and compare it with the destination folder and its subfolders
    for (auto &entry: fs::recursive_directory_iterator(source)) {
        if (entry.is_directory()) {
            auto path = entry.path();
            auto relativePath = fs::relative(path, source);
            auto destinationPath = destination / relativePath;
            if (!fs::exists(destinationPath)) {
                fs::create_directory(destinationPath);
            } else {
                if (sourceTime != sourceTimePrevious) {
                    std::cout << "Source folder has been modified" << std::endl;
                    std::cout << "Source time: " << sourceTime.time_since_epoch().count() << std::endl;
                    std::cout << "sourceTimePrevious time: " << sourceTimePrevious.time_since_epoch().count()
                              << std::endl;

                    fs::remove_all(destinationPath);
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
    std::cout
            << "Hello world ... bo od czegoś trzeba zacząć ... za kilka dni będzie to potężny program do synchronizowania plików xD xD xD\n";

    std::cout << "Source: " << source << std::endl;
    std::cout << "Destination: " << destination << std::endl;
    std::cout << currentPath << std::endl;

    std::mutex mutex;
    std::condition_variable cv;

    do {
        addDiffrenceFileToTemp(source, destination, mutex, cv, true);
        std::cout << "Sleeping..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    } while (true);

    return 0;
}