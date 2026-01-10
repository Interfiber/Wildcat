//
// Created by hstasonis on 1/4/26
//

#pragma once
#include <Wildcat/fs/archive.h>

/**
 * Base class for comma and tab seperated archives
 */
class WildcatDelimiterArchive : public WildcatExternalArchive
{
public:
  WildcatDelimiterArchive(char delimeter);

  void importArchive(const std::string& buffer) override;
  bool isValid(const std::string& buffer) override;

private:
  int getHeaderStartIndex(const std::vector<std::string>& lines);

  char m_delimiter;
};
