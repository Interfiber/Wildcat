//
// Created by hstasonis on 1/10/26
//

#pragma once
#include <nlohmann/json.hpp>
#include "Wildcat/fs/archive.h"
#include "Wildcat/io/channel.h"


/**
 * JSON blob archive
 */
class WildcatJSONArchive : public WildcatExternalArchive
{
public:
  WildcatJSONArchive();

  struct JSONArchive
  {
    /// @brief List of bank names to override the default ones
    std::vector<std::string> overrideBankNames;

    /// @brief  List of channels
    std::vector<std::shared_ptr<WildcatChannel>> channels;
  };

  void importArchive(const std::string& buffer) override;
  bool isValid(const std::string& buffer) override;
  std::string getArchiveName() override;

private:
  /**
   * @param fullLoad When set to `true` new channels will be created from the DEVICE
   */
  std::optional<JSONArchive> loadArchive(nlohmann::json& data, bool fullLoad = false);
};
