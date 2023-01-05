#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;


///
/// \param source
/// \param destination
/// \param mutex
/// \param cv
/// \param isReady
/// @brief The thread is woken up every few seconds and checks the folders, if changes have been made,
/// use lock_guard to avoid race condition.
//function to synchronize two folders
void addDiffrenceFileToTemp(const fs::path &source,
                            const fs::path &destination,
                            std::mutex &mutex,
                            std::condition_variable &cv,
                            bool &isReady) {

    std::unique_lock<std::mutex> lock(mutex); //TODO: Make this function thread safe. Add lock_guard


    for (const auto &entry: fs::directory_iterator(source)) {
        if (fs::is_directory(entry)) {
        } else {

        }
    }
}

int main() {
    std::cout
            << "Hello world ... bo od czegoś trzeba zacząć ... za kilka dni będzie to potężny program do synchronizowania plików xD xD xD\n";

    return 0;
}