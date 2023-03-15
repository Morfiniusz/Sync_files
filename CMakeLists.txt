cmake_minimum_required(VERSION 3.20)
project(Sync_files)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FLAGS -Wall -Wpedantic -Wconversion -Wextra -g)
find_package(Threads REQUIRED)

set(Observer
        ObservedFolder.cpp
        ObservedFolder.h
        ObserverFolder.cpp
        ObserverFolder.h
        ThreadTimer.cpp
        ThreadTimer.h
        SyncFolder.cpp
        SyncFolder.h
        FileCheck.cpp
        FileCheck.h
        ScanFolder.h
        ScanFolder.cpp
        SyncDirectories.cpp
        SyncDirectories.h
        )

add_executable(${PROJECT_NAME} main.cpp ${Observer})
target_compile_options(${PROJECT_NAME} PRIVATE ${FLAGS})
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads ssl crypto)