#include <Wildcat/fs/delimiter.h>

#include <Wildcat/io/channel.h>


WildcatDelimiterArchive::WildcatDelimiterArchive(char delimiter)
{
    m_delimiter = delimiter;
}

void WildcatDelimiterArchive::importArchive(const std::string &buffer)
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
        printf("WildcatPastedSheetArchive::importArchive(...): Skipping line %i, size of %i is not the expected value of 7\n", i, (int) split.size());

        continue;
      }

      // Extract values

      const std::string name = split[0];
      const double freq = std::stod(split[1]);
      const WildcatChannel::ModulationMode modulation = WildcatChannel::stringToModulationMode(split[2]);

      // FIXME: Impl CTCSS/DCS

      const WildcatChannel::LockoutMode lockout = split[4] == "Off" ? WildcatChannel::LockoutMode::Off : WildcatChannel::LockoutMode::Lockout;

      qsizetype delayIndex = WildcatChannel::DELAY_VALUES.indexOf(split[5]);

      if (delayIndex == -1) // No valid delay value found in DELAY_VALUES
      {
        throw std::runtime_error("Invalid delay value: " + split[5]);
      }

      const int delay = WildcatChannel::DELAY_VALUES.at(delayIndex).toInt();

      const WildcatChannel::PriorityMode priority = split[6] == "Off" ? WildcatChannel::PriorityMode::Off : WildcatChannel::PriorityMode::PCH;

      std::shared_ptr<WildcatChannel> newChannel = DEVICE->newChannel(WildcatGlobalState::get()->currentBankIndex);
      newChannel->name = name;
      newChannel->frequency = freq;
      newChannel->modulation = modulation;
      newChannel->lockoutMode = lockout;
      newChannel->delay = delay;
      newChannel->priority = priority;

      // When importing we place all the importer channels in a single bank
      if (newChannel->index > WildcatDevice::MAX_CHANNELS_PER_BANK)
      {
          printf("Skipping import for channel: %i, too many channels in this bank!\n", newChannel->index);

          continue;
      }

      printf("New channel: %s in bank #%i\n", name.c_str(), newChannel->bank);

      WildcatArchiveImporter::get()->channelLoaded(newChannel);
    }
}

bool WildcatDelimiterArchive::isValid(const std::string &buffer)
{
    const std::vector<std::string> lines = Helper_Split(buffer, '\n');

    // We need atleast one line:
    // Header (optional)
    // One frequency line
    if (lines.size() < 1)
    {
        printf("WildcatPastedSheetArchive::isValid(...): Atleast one line required for archive input\n");

        return false;
    }

    int lineStart = getHeaderStartIndex(lines);

    if (lineStart == -1)
    {
        printf("WildcatPastedSheetArchive::isValid(...): getHeaderStartIndex returned -1, archive is not valid!\n");
        return false;
    }

    for (; lineStart < lines.size(); lineStart++)
    {
        const std::string line = lines[lineStart];
        const std::vector<std::string> lineSplit = Helper_Split(line, m_delimiter);

        if (lineSplit.size() != 7)
        {
            printf("WildcatPastedSheetArchive::isValid(...): Line %i only contains %i items, expected 7!\n", lineStart, (int) lineSplit.size());

            return false;
        }

        printf("WildcatPastedSheetArchive::isValid(...): %s\n", line.c_str());
    }

    // All data-related errors can be handled by the import step

    return true;
}

int WildcatDelimiterArchive::getHeaderStartIndex(const std::vector<std::string> &lines)
{
    int lineStart = 0;

    // Check if the header exists, if so skip it during reading
    if (lines.size() >= 2)
    {
        const std::string header = lines.at(0);
        const std::vector<std::string> headerSplit = Helper_Split(header, m_delimiter);
        const std::vector<std::string> headerAssumed = {"Name", "Frequency", "Modulation", "CTCSS/DCS", "Lockout", "Delay", "Priority"};

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
                    printf("WildcatPastedSheetArchive::getHeaderStartIndex(...): Header part %i '%s' is not the expected value of '%s'\n", i, found.c_str(), assumed.c_str());

                    isHeaderValid = false;
                }
            }

            if (isHeaderValid)
            {
                printf("WildcatPastedSheetArchive::getHeaderStartIndex(...): Valid header found in pasted sheet!\n");

                lineStart = 1; // Skip header during parsing
            }
        }
    }

    return lineStart;
}
