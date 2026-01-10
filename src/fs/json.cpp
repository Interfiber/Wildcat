#include "Wildcat/fs/json.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

WildcatJSONArchive::WildcatJSONArchive() = default;

std::string
WildcatJSONArchive::getArchiveName()
{
  return "Wildcat JSON";
}

void
WildcatJSONArchive::importArchive(const std::string& buffer)
{
}

bool
WildcatJSONArchive::isValid(const std::string& buffer)
{
  json data;
  try
  {
    data = json::parse(buffer, nullptr, true, true);
  }
  catch (std::exception& e)
  {
    spdlog::error("Exception occured in JSON parsing, archive invalid: {}", e.what());

    return false;
  }

  return loadArchive(data).has_value();
}


std::optional<WildcatJSONArchive::JSONArchive>
WildcatJSONArchive::loadArchive(nlohmann::json& data)
{
  JSONArchive ar{};

  json overrideBanks = data["$override_banks"];
  json channels = data["$channels"];

  if (overrideBanks.is_null())
  {
    spdlog::error("'$override_banks' is null");
  }

  if (channels.is_null() || !channels.is_array())
  {
    spdlog::error("Archive is missing '$channels' (or not an array), cannot load");

    return std::optional<JSONArchive>();
  }

  for (const json& channel : channels)
  {
    json bank = channel["bank"]; // Integer

    json name = channel["name"]; // String
    json frequency = channel["frequency"]; // Float
    json modulation = channel["modulation"]; // String
    json ctcssDcs = channel["ctcss_dcs"]; // String
    json lockout = channel["lockout"]; // Boolean
    json delay = channel["delay"]; // Integer
    json priority = channel["priority"]; // String

    if (name.is_null() || frequency.is_null() || modulation.is_null() || ctcssDcs.is_null() || lockout.is_null()
        || delay.is_null() || priority.is_null())
    {
      spdlog::error("Wildcat JSON payload missing some or all keys!");
    }
  }

  return std::make_optional<JSONArchive>(ar);
}
