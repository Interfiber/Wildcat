//
// Created by hstasonis on 12/31/25.
//

#include <Wildcat/fs/pastedsheet.h>
#include <Wildcat/global.h>
#include <Wildcat/ui/channelswidget.h>

WildcatPastedSheetArchive::WildcatPastedSheetArchive() : WildcatDelimiterArchive('\t') {}

std::string
WildcatPastedSheetArchive::getArchiveName()
{
  return "Tab Separated Values (TSV)";
}
