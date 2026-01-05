//
// Created by hstasonis on 12/31/25.
//

#include <Wildcat/fs/csv.h>


WildcatCSVArchive::WildcatCSVArchive() : WildcatDelimiterArchive(',')
{

}

std::string WildcatCSVArchive::getArchiveName()
{
    return "Comma Separated Values (CSV)";
}
