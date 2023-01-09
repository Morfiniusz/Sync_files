cmake_minimum_required(VERSION 3.20)
project(Sync_files)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FLAGS -Wall -Wpedantic -Werror -Wconversion -Wextra -O3)

set(Observer
        ObservedFolder.cpp
        ObservedFolder.h
        ObserverFolder.cpp
        ObserverFolder.h
        ThreadTimer.cpp
        ThreadTimer.h
        )

add_executable(${PROJECT_NAME} main.cpp ${Observer})
target_compile_options(${PROJECT_NAME} PRIVATE ${FLAGS})