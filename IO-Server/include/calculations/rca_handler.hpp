#pragma once
#include "calculation_handler.hpp"

// ─── RCA – Rate of Change Analysis ───────────────────────────────────────────
//
//  Formula:  ROC(i) = (close[i] - close[i - window]) / close[i - window] * 100
//
//  Required params:  symbol, timeframe
//  Optional params:  window (default 14), from_time, to_time, limit
//
class RCAHandler : public ICalculationHandler
{
public:
    crow::json::wvalue compute(const CalcParams &params, SQLiteDB &db) override;

    std::string name() const override { return "rca"; }

    std::string description() const override
    {
        return "Rate of Change Analysis: momentum indicator measuring the "
               "percentage price change over a configurable rolling window.";
    }
};
