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
    const std::string freq = Helper_Split(str, ' ')[1];

    // Find correct DCS code to use
    for (int i = 0; i < DCS_CODES.size(); i++)
    {
      const int code = DCS_CODES[i];

      if (std::stoi(freq) == code)
        return DCS_START_CODE + i;
    }
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

  spdlog::warn("Invalid CTCSS/DCS code for Wildcat_GetCTCSSCode");

  return NONE_CODE;
}

std::string
Wildcat_GetCTCSSName(int code)
{
  if (code > CTCSS_START_CODE && code < DCS_START_CODE)
  {
    const std::string ctcss = CTCSS_CODES[code - CTCSS_START_CODE];

    return "CTCSS " + ctcss;
  }
  else if (code > DCS_START_CODE
           && code < DCS_CODES[DCS_CODES.size() - 1] + 1) // Weird maximum calculation here, whatever.
  {
    const int dcs = DCS_CODES[code - DCS_START_CODE];

    return "DCS " + std::to_string(dcs);
  }
  else if (code == NONE_CODE)
  {
    return "None / All";
  }
  else if (code == SEARCH_CODE)
  {
    return "Search";
  }
  else if (code == NO_TONE_CODE)
  {
    return "No tone";
  }

  spdlog::error("Invalid input code for Wildcat_GetCTCSSName: {}", code);

  return "None / All";
}
