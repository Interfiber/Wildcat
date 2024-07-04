#pragma once
#include "filedialogs.h"
#include <string>
#include <vector>

inline std::vector<std::string> Helper_Split(const std::string &s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
    // elems.push_back(std::move(item)); // if C++11 (based on comment from
    // @mchiasson)
  }
  return elems;
}

inline void Helper_ErrorMsg(const std::string &msg) {
  pfd::message dialog("Wildcat Error", msg, pfd::choice::ok, pfd::icon::error);
}