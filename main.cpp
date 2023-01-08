#include <iostream>
#include <filesystem>
#include "ObservedFolder.h"
#include "ObserverFolder.h"

namespace fs = std::filesystem;

enum class MENU_OPTIONS {
    RUN_ONCE = 1,
    AUTO_SYNC_ON,
    AUTO_SYNC_OFF,
    END
};

const fs::path currentPath = fs::current_path();
const fs::path sourcePath = currentPath.parent_path() / "Test/TestFolder/MasterFolder";
const fs::path destinationPath = currentPath.parent_path() / "Test/TestFolder/DestinationFolder";


void showOptions() {
    std::cout << "1. Run sync once" << std::endl;
    std::cout << "2. Start auto sync [100 ms]" << std::endl;
    std::cout << "3. Stop auto sync [100 ms]" << std::endl;
    std::cout << "4. End" << std::endl;
}

void displayMenu(ObservedFolder &observedFolder) {
    int option = 0;
    MENU_OPTIONS menuOptions{};
    while (menuOptions != MENU_OPTIONS::END) {
        showOptions();
        //cast cin to MENU_OPTION
        std::cin >> option;
        menuOptions = static_cast<MENU_OPTIONS>(option);
        switch (menuOptions) {
            case MENU_OPTIONS::RUN_ONCE: {
                observedFolder.notifyObservers();
                break;
            }

            case MENU_OPTIONS::AUTO_SYNC_ON: {
                observedFolder.autoCheckForChangesStart();
                break;
            }

            case MENU_OPTIONS::AUTO_SYNC_OFF: {
                observedFolder.autoCheckForChangesStop();
                break;
            }

            case MENU_OPTIONS::END: {
//                stop thread responsible for auto sync of folders
                observedFolder.autoCheckForChangesStop();

                observedFolder.autoCheckThread_.join();
                std::cout << "End" << std::endl;
                break;
            }

            default:
                std::cout << "Unimplemented function" << std::endl;
                break;
        }
    }
}


int main() {
    ObservedFolder observedFolder(sourcePath.string());
    ObserverFolder observerFolder(destinationPath.string(), &observedFolder);
    //You can add more observers to the observed folder!
    observedFolder.registerObserver(&observerFolder);

    //display menu in infinite loop
    displayMenu(observedFolder);

    return 0;
}