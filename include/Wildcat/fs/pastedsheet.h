//
// Created by hstasonis on 1/4/26
//

#pragma once
#include "Wildcat/fs/delimiter.h"

/**
 * Paste from google sheets, excel, etc
 */
class WildcatPastedSheetArchive : public WildcatDelimiterArchive
{
public:
    WildcatPastedSheetArchive();

    [[nodiscard]] std::string getArchiveName() override;

private:
};

