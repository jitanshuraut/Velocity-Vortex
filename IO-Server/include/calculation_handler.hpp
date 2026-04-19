#pragma once
#include <string>
#include <map>
#include <cstdint>
#include <crow.h>
#include "SQLiteDB.hpp"

// ─── Query parameters passed to every calculation ─────────────────────────────
struct CalcParams
{
    std::string symbol;    // primary symbol, e.g. "BTCUSDT"
    std::string symbol2;   // secondary symbol (for cross-asset methods)
    std::string timeframe; // "1m" | "5m" | "1h" | "1d" …
    int64_t from_time = 0; // unix-ms lower bound (0 = no filter)
    int64_t to_time = 0;   // unix-ms upper bound (0 = no filter)
    int window = 14;       // rolling window length
    int limit = 200;       // max output rows

    // Method-specific extras (e.g. {"multiplier": "2"} for custom EMA)
    std::map<std::string, std::string> extra;
};

// ─── Extensible calculation interface ────────────────────────────────────────
//
// To add a new calculation method:
//   1.  Create a class that inherits ICalculationHandler.
//   2.  Implement compute(), name(), and description().
//   3.  Register an instance with CalculationRegistry::register_handler().
//
class ICalculationHandler
{
public:
    virtual ~ICalculationHandler() = default;

    // Execute the computation and return a JSON response body.
    // On validation error, return {"error": "<message>"}.
    virtual crow::json::wvalue compute(const CalcParams &params,
                                       SQLiteDB &db) = 0;

    // Unique lowercase identifier used in the URL: /api/calc/<name>
    virtual std::string name() const = 0;

    // One-sentence description exposed via GET /api/calc
    virtual std::string description() const = 0;
};
