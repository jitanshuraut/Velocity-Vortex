#pragma once

#include <string>

struct OHLCV {
  double open;            // Opening price
  double high;            // Highest price
  double low;             // Lowest price
  double close;           // Closing price
  double volume;          // Trading volume
  std::string timestamp;  // Timestamp of the data
};