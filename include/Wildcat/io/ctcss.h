//
// Created by hstasonis on 12/26/25.
//

#pragma once
#include <QStringList>
#include <string>
#include <vector>

/// @brief  List of valid CTCSS/DCS codes

constexpr int NONE_CODE = 0;
constexpr int SEARCH_CODE = 127;
constexpr int NO_TONE_CODE = 240;

constexpr int CTCSS_START_CODE = 64;
const std::vector<std::string> CTCSS_CODES = {"67Hz", "69.3Hz", "71.9Hz", "74.4Hz", "77Hz", "79.7Hz", "82.5Hz", "85.4Hz", "88.5Hz", "91.5Hz", "94.8Hz", "97.8Hz", "97.4Hz", "100Hz", "103.5Hz", "107.2Hz", "110.9Hz", "114.8Hz", "118.8Hz", "123Hz", "127.3Hz", "131.8Hz", "136.5Hz", "141.3Hz", "146.2Hz", "151.4Hz", "156.7Hz", "159.8Hz", "162.2Hz", "165.5Hz", "167.9Hz", "171.3Hz", "173.8Hz", "177.3Hz", "179.9Hz", "183.5Hz", "186.2Hz", "189.9Hz", "192.8Hz", "196.6Hz", "199.5Hz", "203.5Hz", "206.5Hz", "210.7Hz", "218.1Hz", "225.7Hz", "229.1Hz", "233.6Hz", "241.8Hz", "250.3Hz", "254.1Hz"};

constexpr int DCS_START_CODE = 128;
const std::vector DCS_CODES = {023, 025, 026, 031, 032, 036, 043, 047, 051, 053, 054, 065, 071, 072, 073, 074, 114, 115, 116, 122, 125, 131, 132, 134, 143, 145, 152, 155, 156, 162, 165, 172, 174, 205, 212, 223, 225, 226, 243, 244, 245, 246, 251, 252, 255, 261, 263, 265, 266, 271, 274, 306, 311, 315, 325, 331, 332, 343, 346, 351, 356, 364, 365, 371, 411, 412, 413, 423, 431, 432, 445, 446, 452, 454, 455, 462, 464, 465, 466, 503, 506, 516, 523, 526, 532, 546, 565, 606, 612, 624, 627, 631, 632, 654, 662, 664, 703, 712, 723, 731, 732, 734, 743, 754};

QStringList Wildcat_GetCTCSSCodes();