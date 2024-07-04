#include "wildcatsavefile.h"
#include <SDL_log.h>
#include <fstream>

WildcatSaveFile::WildcatSaveFile() {}

void WildcatSaveFile::writeToDisk(const std::filesystem::path &file) {
  std::ofstream ofs(file, std::ios::binary);

  ofs.write((char *)&header, sizeof(header));
  ofs.write((char *)&version, sizeof(version));

  for (auto &channel : channels) {
    size_t cNameSize = channel.name.size();

    ofs.write((char *) &cNameSize, sizeof(size_t));

    ofs.write((char *)channel.name.c_str(), cNameSize);

    ofs.write((char *)&channel.frequency, sizeof(float));
    ofs.write((char *)&channel.mod, sizeof(int));
    ofs.write((char *)&channel.lockout, sizeof(int));
    ofs.write((char *)&channel.delay, sizeof(int));
    ofs.write((char *)&channel.priority, sizeof(int));
  }

  ofs.close();

  SDL_Log("Saved data to: %s", file.c_str());
}

void WildcatSaveFile::readFromDisk(const std::filesystem::path &file) {}
