#include "Wildcat/fs/json.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "Wildcat/global.h"
#include "Wildcat/io/channel.h"

#define check_exists(key, json)                                                                                        \
  if (!json.contains(key))                                                                                             \
  {                                                                                                                    \
    spdlog::error("JSON object is missing required key: {}", key);                                                     \
    continue;                                                                                                          \
  }

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
  // No try catch here since it should be caught by the parent function

  json data = json::parse(buffer, nullptr, true, true);

  std::optional<JSONArchive> archive = loadArchive(data, true);

  if (!archive.has_value())
  {
    throw std::runtime_error("Failed to load JSON archive, loadArchive failed");
  }

  // Add channels from the archive to the device / UI
  for (auto& channel : archive->channels)
  {
    // Copied from delimiter.cpp

    if (channel->index > WildcatDevice::MAX_CHANNELS_PER_BANK)
    {
      spdlog::warn("Skipping import for channel: {}, too many channels in this bank!", channel->index);

      continue;
    }

    WildcatArchiveImporter::get()->channelLoaded(channel);
  }
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
WildcatJSONArchive::loadArchive(nlohmann::json& data, bool fullLoad)
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
    check_exists("bank", channel);
    check_exists("name", channel);
    check_exists("modulation", channel);
    check_exists("ctcss_dcs", channel);
    check_exists("lockout", channel);
    check_exists("delay", channel);
    check_exists("priority", channel);

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

      continue;
    }

    // Basic type checks

    if (!bank.is_number_unsigned())
    {
      spdlog::error("'bank' must be an unsigned integer");
      continue;
    }

    if (!name.is_string())
    {
      spdlog::error("'name' must be a string");
      continue;
    }

    if (!frequency.is_number())
    {
      spdlog::error("'frequency' must be a number");
      continue;
    }

    if (!modulation.is_string())
    {
      spdlog::error("'modulation' must be a string");
      continue;
    }

    // FIXME: CTCSS/DCS

    if (!lockout.is_boolean())
    {
      spdlog::error("'lockout' must be a boolean");
      continue;
    }

    if (!priority.is_string())
    {
      spdlog::error("'priority' must be a string");
      continue;
    }


    // Load the channel information in, ID information is left blank until we copy it into the device

    std::shared_ptr<WildcatChannel> wChan = nullptr;

    if (fullLoad)
    {
      wChan = DEVICE->newChannel(bank.get<int>());
    }
    else
    {
      wChan = std::make_shared<WildcatChannel>();
    }

    wChan->bank = bank.get<int>();
    wChan->name = name.get<std::string>();
    wChan->frequency = frequency.get<float>();
    wChan->modulation = WildcatChannel::stringToModulationMode(modulation.get<std::string>());

    // FIXME: CTCSS/DCS

    wChan->lockoutMode = lockout.get<bool>() ? WildcatChannel::LockoutMode::Lockout : WildcatChannel::LockoutMode::Off;
    wChan->delay = delay.get<int>();
    wChan->priority
      = priority.get<std::string>() == "PCH" ? WildcatChannel::PriorityMode::PCH : WildcatChannel::PriorityMode::Off;

    ar.channels.push_back(wChan);

    spdlog::debug("Loaded channel with name '{}'", wChan->name);
  }

  return std::make_optional<JSONArchive>(ar);
}
