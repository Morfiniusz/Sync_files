cmake_minimum_required(VERSION 3.20)
project(Sync_files)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FLAGS -Wall -Wpedantic -Werror -Wconversion -Wextra -O3)

set(TEST_SOURCES
        ./tests/main_test.cpp
        )

set(PROJECT_SOURCES
        main.cpp
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
        ThreadPool.cpp
        ThreadPool.h
        )

# Add Google Test as a subproject
add_subdirectory(tests/googletest)

# Find required packages
find_package(Threads REQUIRED)

# App
add_executable(${PROJECT_NAME}_app ${PROJECT_SOURCES})
target_compile_options(${PROJECT_NAME}_app PRIVATE ${FLAGS})
target_link_libraries(${PROJECT_NAME}_app PRIVATE Threads::Threads ssl crypto)

# Tests
add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})
target_compile_options(${PROJECT_NAME}_tests PRIVATE ${FLAGS})
target_link_libraries(${PROJECT_NAME}_tests PRIVATE gtest gtest_main Threads::Threads)

# Enable testing for this directory
enable_testing()

# Add the test executable to CTest
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}_tests)
