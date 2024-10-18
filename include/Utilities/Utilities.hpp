#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include "./OHLCV.hpp"
#include "./Bar.hpp"

OHLCV barToOHLCV(const Bar& bar);

Bar ohlcvToBar(const OHLCV& ohlcv, const std::string& symbol);

OHLCV buildAggregatedCandle(const std::vector<OHLCV>& candles);

#endif 
