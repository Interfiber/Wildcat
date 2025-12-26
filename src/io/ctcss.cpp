//
// Created by hstasonis on 12/26/25.
//

#include <Wildcat/io/ctcss.h>

QStringList Wildcat_GetCTCSSCodes()
{
  QStringList result;

  result.push_back("None / All"); // 0
  result.push_back("Search"); // 127
  result.push_back("No tone"); // 240

  for (const std::string &code : CTCSS_CODES)
  {
    result.push_back(QString::fromStdString("CTCSS " + code));
  }

  for (const int &code : DCS_CODES)
  {
    result.push_back(QString::fromStdString("DCS " + std::to_string(code)));
  }

  return result;
}
