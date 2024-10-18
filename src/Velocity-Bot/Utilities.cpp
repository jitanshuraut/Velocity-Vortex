#include "Utilities/Utilities.hpp"

OHLCV barToOHLCV(const Bar& bar) {
    OHLCV ohlcv;
    ohlcv.open = bar.open;
    ohlcv.high = bar.high;
    ohlcv.low = bar.low;
    ohlcv.close = bar.close;
    ohlcv.volume = static_cast<double>(bar.volume);
    ohlcv.timestamp = bar.time;  

    return ohlcv;
}

Bar ohlcvToBar(const OHLCV& ohlcv, const std::string& symbol) {
    Bar bar(symbol);
    bar.open = ohlcv.open;
    bar.high = ohlcv.high;
    bar.low = ohlcv.low;
    bar.close = ohlcv.close;
    bar.volume = static_cast<long>(ohlcv.volume);
    bar.time = ohlcv.timestamp;  

    return bar;
}

OHLCV buildAggregatedCandle(const std::vector<OHLCV>& candles) {
    OHLCV aggregatedCandle;
    if (candles.empty()) return aggregatedCandle;

    aggregatedCandle.open = candles.front().open;
    aggregatedCandle.close = candles.back().close;
    aggregatedCandle.high = std::max_element(candles.begin(), candles.end(),
        [](const OHLCV& a, const OHLCV& b) { return a.high < b.high; })->high;
    aggregatedCandle.low = std::min_element(candles.begin(), candles.end(),
        [](const OHLCV& a, const OHLCV& b) { return a.low < b.low; })->low;
    aggregatedCandle.volume = std::accumulate(candles.begin(), candles.end(), 0.0,
        [](double total, const OHLCV& candle) { return total + candle.volume; });
    aggregatedCandle.timestamp = candles.back().timestamp;

    return aggregatedCandle;
}
