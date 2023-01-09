#include <iostream>
#include <filesystem>
#include <future>
#include "ObservedFolder.h"
#include "ObserverFolder.h"


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
ObserverFolder observerFolder(destinationPath, &observedFolder);
ObserverFolder observerFolder2(destinationPath2, &observedFolder);

void displayMenu() {
    std::cout << "1. Run sync once" << std::endl;
    std::cout << "2. Start auto sync [100 ms]" << std::endl;
    std::cout << "3. Stop auto sync [100 ms]" << std::endl;
    std::cout << "4. End" << std::endl;
}

void mainMenu(ObservedFolder &observedFolderObj) {
    int option = 0;
    MENU_OPTIONS menuOptions{};
    while (menuOptions != MENU_OPTIONS::END) {
        displayMenu();
        //cast cin to MENU_OPTION
        std::cin >> option;
        menuOptions = static_cast<MENU_OPTIONS>(option);
        switch (menuOptions) {
            case MENU_OPTIONS::SYNC_ONCE: {
                observedFolderObj.notifyObservers();
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
                threadRun.store(false);
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
            observedFolder.checkForChanges();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main() {

    std::thread runDiffThread(runDiff);

    //You can add more observers to the observed folder!
    observedFolder.registerObserver(&observerFolder);
    observedFolder.registerObserver(&observerFolder2);

    //Make global observer for all folders
//    observedFolder.registerObserver(&globalObserver);

    //display menu in infinite loop
    mainMenu(observedFolder);
    runDiffThread.join();

    return 0;
}