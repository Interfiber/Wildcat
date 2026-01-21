//
// Created by hstasonis on 12/26/25.
//

#include <Wildcat/io/ctcss.h>
#include <spdlog/spdlog.h>
#include "Wildcat/fs/archive.h"

bool
Util_BeginsWith(const std::string& in, const std::string& sub)
{
  if (sub.size() > in.size())
    return false;

  if (sub.size() == in.size())
    return in == sub;

  for (int i = 0; i < sub.size(); i++)
  {
    if (in[i] != sub[i])
      return false;
  }

  return true;
}

QStringList
Wildcat_GetCTCSSCodes()
{
  QStringList result;

  result.push_back("None / All"); // 0
  result.push_back("Search"); // 127
  result.push_back("No tone"); // 240

  for (const std::string& code : CTCSS_CODES)
  {
    result.push_back(QString::fromStdString("CTCSS " + code));
  }

  for (const int& code : DCS_CODES)
  {
    result.push_back(QString::fromStdString("DCS " + std::to_string(code)));
  }

  return result;
}

int
Wildcat_GetCTCSSCode(const std::string& str)
{
  if (Util_BeginsWith(str, "CTCSS"))
  {
    const std::string freq = Helper_Split(str, ' ')[1];

    // Find correct CTCSS code to use
    for (int i = 0; i < CTCSS_CODES.size(); i++)
    {
      const std::string code = CTCSS_CODES[i];

      if (code.find(freq) != std::string::npos) // Code found, return it plus ctcss start index
        return CTCSS_START_CODE + i;
    }

    spdlog::error("Failed to find CTCSS code for: {}, defaulting to NONE_CODE", str);

    return NONE_CODE;
  }
  else if (Util_BeginsWith(str, "DCS"))
  {
  }
  // Special names
  else if (str == "None / All")
  {
    return NONE_CODE;
  }
  else if (str == "Search")
  {
    return SEARCH_CODE;
  }
  else if (str == "No tone")
  {
    return NO_TONE_CODE;
  }

  throw std::runtime_error("Invalid CTCSS/DCS code for Wildcat_GetCTCSSCode");
}
