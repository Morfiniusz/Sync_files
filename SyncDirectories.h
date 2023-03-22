#ifndef SYNC_DIRECTORIES_H
#define SYNC_DIRECTORIES_H
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <utility>
#include <vector>
#include "ScanFolder.h"

extern const std::filesystem::path mainFolderPath;
class SyncDirectories : public ScanItem {
public:

SyncDirectories()
{
    std::cout << "C'tor called\n";
};

~SyncDirectories(){
    std::cout << "D'tor called\n";
};

void printPairs (std::vector<std::pair<size_t, std::vector<ScanItem>>> );
void printVec(const std::vector<ScanItem>& );
void printMap(const std::unordered_map<std::string, ScanItem>& );
auto pathFinder(const std::filesystem::path& );
auto stateCreator(const std::vector<std::pair<size_t,std::filesystem::path>>& );
void syncDirectories(size_t idx);
// size_t getIdx() const;
// void setIdx(size_t );

private: 
    //size_t idx_ {0};
    // std::vector<std::pair<size_t, std::filesystem::path>> vecOfPaths_;
    // std::vector<std::pair<size_t, std::vector<ScanItem>>> vecOfStates_;
    // std::unordered_map<std::string, ScanItem> mapOfItemsToCopy_;
};


#endif