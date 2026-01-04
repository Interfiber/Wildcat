//
// Created by hstasonis on 1/4/26
//

#pragma once
#include <Wildcat/fs/delimiter.h>

class WildcatCSVArchive : public WildcatDelimiterArchive
{
public:
    WildcatCSVArchive();

    [[nodiscard]] std::string getArchiveName() override;
};
