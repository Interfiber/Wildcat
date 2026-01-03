//
// Created by hstasonis on 12/31/25.
//

#include <Wildcat/fs/archive.h>


void WildcatCSVArchive::importArchive(const std::string& buffer)
{

}

std::string WildcatCSVArchive::getArchiveName()
{
    return "Comma Separated Values (CSV)";
}

bool WildcatCSVArchive::isValid(const std::string& buffer)
{
    return false;
}
