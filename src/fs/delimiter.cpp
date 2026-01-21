#include <Wildcat/fs/delimiter.h>

#include <Wildcat/io/channel.h>
#include <spdlog/spdlog.h>
#include "Wildcat/io/ctcss.h"


WildcatDelimiterArchive::WildcatDelimiterArchive(char delimiter) { m_delimiter = delimiter; }

void
WildcatDelimiterArchive::importArchive(const std::string& buffer)
{
  const std::vector<std::string> lines = Helper_Split(buffer, '\n');

  if (lines.size() < 1)
  {
    throw std::runtime_error("Atleast one line required for archive input\n");
  }

  int headerIndex = getHeaderStartIndex(lines);

  // Loop over all the lines (channels)

  for (int i = headerIndex; i < lines.size(); i++)
  {
    const std::vector<std::string> split = Helper_Split(lines[i], m_delimiter);

    if (split.size() != 7)
    {
      spdlog::error("Skipping line {}, size of {} is not the expected value of 7", i, (int)split.size());

      continue;
    }

    // Extract values

    const std::string name = split[0];
    const double freq = std::stod(split[1]);
    const WildcatChannel::ModulationMode modulation = WildcatChannel::stringToModulationMode(split[2]);
    const int ctcssDcsCode = Wildcat_GetCTCSSCode(split[3]);

    const WildcatChannel::LockoutMode lockout
      = split[4] == "Off" ? WildcatChannel::LockoutMode::Off : WildcatChannel::LockoutMode::Lockout;

    qsizetype delayIndex = WildcatChannel::DELAY_VALUES.indexOf(split[5]);

    if (delayIndex == -1) // No valid delay value found in DELAY_VALUES
    {
      throw std::runtime_error("Invalid delay value: " + split[5]);
    }

    const int delay = WildcatChannel::DELAY_VALUES.at(delayIndex).toInt();

    const WildcatChannel::PriorityMode priority
      = split[6] == "Off" ? WildcatChannel::PriorityMode::Off : WildcatChannel::PriorityMode::PCH;

    std::shared_ptr<WildcatChannel> newChannel = DEVICE->newChannel(WildcatGlobalState::get()->currentBankIndex);
    newChannel->name = name;
    newChannel->frequency = freq;
    newChannel->modulation = modulation;
    newChannel->ctcss = ctcssDcsCode;
    newChannel->lockoutMode = lockout;
    newChannel->delay = delay;
    newChannel->priority = priority;

    // When importing we place all the importer channels in a single bank
    if (newChannel->index > WildcatDevice::MAX_CHANNELS_PER_BANK)
    {
      spdlog::warn("Skipping import for channel: {}, too many channels in this bank!", newChannel->index);

      continue;
    }

    spdlog::info("New channel: {} in bank #{}", name, newChannel->bank);

    WildcatArchiveImporter::get()->channelLoaded(newChannel);
  }
}

bool
WildcatDelimiterArchive::isValid(const std::string& buffer)
{
  const std::vector<std::string> lines = Helper_Split(buffer, '\n');

  // We need atleast one line:
  // Header (optional)
  // One frequency line
  if (lines.size() < 1)
  {
    spdlog::error("Atleast one line required for archive input");

    return false;
  }

  int lineStart = getHeaderStartIndex(lines);

  if (lineStart == -1)
  {
    spdlog::error("getHeaderStartIndex returned -1, archive is not valid!");
    return false;
  }

  for (; lineStart < lines.size(); lineStart++)
  {
    const std::string line = lines[lineStart];
    const std::vector<std::string> lineSplit = Helper_Split(line, m_delimiter);

    if (lineSplit.size() != 7)
    {
      spdlog::error("Line {} only contains {} items, expected 7!", lineStart, (int)lineSplit.size());

      return false;
    }

    spdlog::trace("{} {}", lineStart, line);
  }

  // All data-related errors can be handled by the import step

  return true;
}

int
WildcatDelimiterArchive::getHeaderStartIndex(const std::vector<std::string>& lines)
{
  int lineStart = 0;

  // Check if the header exists, if so skip it during reading
  if (lines.size() >= 2)
  {
    const std::string header = lines.at(0);
    const std::vector<std::string> headerSplit = Helper_Split(header, m_delimiter);
    const std::vector<std::string> headerAssumed
      = { "Name", "Frequency", "Modulation", "CTCSS/DCS", "Lockout", "Delay", "Priority" };

    if (headerSplit.size() != 7)
    {
      return -1;
    }
    else
    {
      // Compare the expected header to the found one

      bool isHeaderValid = true;

      for (int i = 0; i < headerAssumed.size(); i++)
      {
        const std::string assumed = headerAssumed[i];
        const std::string found = headerSplit[i];

        if (assumed != found)
        {
          spdlog::error("Header part {} '{}' is not the expected value "
                        "of '{}'\n",
                        i, found, assumed);

          isHeaderValid = false;
        }
      }

      if (isHeaderValid)
      {
        spdlog::debug("Valid header found in pasted sheet!");

        lineStart = 1; // Skip header during parsing
      }
    }
  }

  return lineStart;
}
