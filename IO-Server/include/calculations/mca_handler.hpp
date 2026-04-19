#pragma once
#include "calculation_handler.hpp"

// ─── MCA – Moving Correlation Analysis ───────────────────────────────────────
//
//  Computes the rolling Pearson correlation coefficient between the close
//  prices of two symbols over a sliding window.
//
//  Required params:  symbol, symbol2, timeframe
//  Optional params:  window (default 20), from_time, to_time, limit
//
class MCAHandler : public ICalculationHandler
{
public:
    crow::json::wvalue compute(const CalcParams &params, SQLiteDB &db) override;

    std::string name() const override { return "mca"; }

    std::string description() const override
    {
        return "Moving Correlation Analysis: rolling Pearson correlation "
               "between two symbols' close prices over a configurable window.";
    }
};
