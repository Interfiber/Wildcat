#include "wildcatsavefile.h"
#include "helpers.h"
#include "scanner.hpp"
#include <SDL_log.h>
#include <SDL_messagebox.h>
#include <fstream>

WildcatSaveFile::WildcatSaveFile() {}

void WildcatSaveFile::writeToDisk(const std::filesystem::path &file) {
  std::ofstream ofs(file, std::ios::binary);

  ofs.write((char *)&header, sizeof(header));
  ofs.write((char *)&version, sizeof(version));

  for (auto &channel : channels) {
    size_t cNameSize = channel.name.size() + 1;

    ofs.write((char *)&cNameSize, sizeof(size_t));

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

void WildcatSaveFile::readFromDisk(const std::filesystem::path &file) {
  std::ifstream ifs(file, std::ios::binary);

  ifs.read((char *)&header, sizeof(header));

  if (header != SCANNER_SAVE_DATA_HEADER) {
    Helper_ErrorMsg("Invalid header in save file: " + file.generic_string());

    std::exit(EXIT_FAILURE);
  }

  ifs.read((char *)&version, sizeof(version));

  if (version != SCANNER_SAVE_DATA_VERSION) {
    Helper_ErrorMsg("Invalid version in save file: " + file.generic_string());

    std::exit(EXIT_FAILURE);
  }

  for (int i = 0; i < SCANNER_CHANNELS; i++) {
    auto &channel = channels[i];

    size_t cNameSize = channel.name.size();

    ifs.read((char *)&cNameSize, sizeof(size_t));

    char *name = (char *)malloc(cNameSize);

    ifs.read((char *)name, cNameSize);

    channel.name = name;

    free(name);

    ifs.read((char *)&channel.frequency, sizeof(float));
    ifs.read((char *)&channel.mod, sizeof(int));
    ifs.read((char *)&channel.lockout, sizeof(int));
    ifs.read((char *)&channel.delay, sizeof(int));

    if (channel.delay == -10) {
      channel.internalState.delaySelected = 0;
    } else if (channel.delay == -5) {
      channel.internalState.delaySelected = 1;
    } else {
      channel.internalState.delaySelected = channel.delay + 2;
    }

    ifs.read((char *)&channel.priority, sizeof(int));
  }

  ifs.close();

  SDL_Log("Loaded data from: %s", file.c_str());
}
