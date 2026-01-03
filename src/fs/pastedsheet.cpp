//
// Created by hstasonis on 12/31/25.
//

#include <Wildcat/fs/archive.h>

void WildcatPastedSheetArchive::importArchive(const std::string& buffer)
{
}

std::string WildcatPastedSheetArchive::getArchiveName()
{
    return "Spreadsheet";
}

bool WildcatPastedSheetArchive::isValid(const std::string& buffer)
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

    int lineStart = 0;

    // Check if the header exists, if so skip it during reading
    if (lines.size() >= 2)
    {
        const std::string header = lines.at(0);
        const std::vector<std::string> headerSplit = Helper_Split(header, '\t');
        const std::vector<std::string> headerAssumed = {"Name", "Frequency", "Modulation", "CTCSS/DCS", "Lockout", "Delay", "Priority"};

        if (headerSplit.size() != 7)
        {
            printf("WildcatPastedSheetArchive::isValid(...): During header pre-check the first line was determined to be invalid (size != 7), this document is not a parsable.\n");

            return false;
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
                    printf("WildcatPastedSheetArchive::isValid(...): Header part %i '%s' is not the expected value of '%s'\n", i, found.c_str(), assumed.c_str());

                    isHeaderValid = false;
                }
            }

            if (isHeaderValid)
            {
                printf("WildcatPastedSheetArchive::isValid(...): Valid header found in pasted sheet!\n");

                lineStart = 1; // Skip header during parsing
            }
        }
    }

    for (; lineStart < lines.size(); lineStart++)
    {
        const std::string line = lines[lineStart];
        const std::vector<std::string> lineSplit = Helper_Split(line, '\t');

        if (lineSplit.size() != 7)
        {
            printf("WildcatPastedSheetArchive::isValid(...): Line %i only contains %i items, expected 7!\n", lineStart, (int) lineSplit.size());

            return false;
        }
    }

    // All data-related errors can be handled by the import step

    return true;
}
