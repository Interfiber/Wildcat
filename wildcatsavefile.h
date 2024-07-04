#ifndef WILDCATSAVEFILE_H
#define WILDCATSAVEFILE_H

#include "scanner.hpp"
#include "wildcatchannel.h"
#include <array>
#include <filesystem>

class WildcatSaveFile {
public:
    WildcatSaveFile();

    int header, version;

    std::array<WildcatChannel, SCANNER_CHANNELS> channels;

    void writeToDisk(const std::filesystem::path &file);

    void readFromDisk(const std::filesystem::path &file);
};

#endif // WILDCATSAVEFILE_H
