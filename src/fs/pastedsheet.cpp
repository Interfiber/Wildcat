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
    return true;
}
