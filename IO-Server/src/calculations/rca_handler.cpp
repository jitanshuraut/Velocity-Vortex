#include "calculations/rca_handler.hpp"
#include <vector>
#include <algorithm>
#include <stdexcept>

crow::json::wvalue RCAHandler::compute(const CalcParams &params, SQLiteDB &db)
{
    // ── Validate inputs ───────────────────────────────────────────────────────
    if (params.symbol.empty())
    {
        crow::json::wvalue err;
        err["error"] = "symbol is required";
        return err;
    }
    if (params.timeframe.empty())
    {
        crow::json::wvalue err;
        err["error"] = "timeframe is required";
        return err;
    }

    const int window = params.window > 0 ? params.window : 14;
    const int limit = params.limit > 0 ? params.limit : 200;

    // ── Fetch raw OHLCV (need `window` extra rows before the output range) ───
    std::string sql = R"(
        SELECT o.open_time, o.close
        FROM   ohlcv o
        JOIN   symbols s ON s.symbol_id = o.symbol_id
        WHERE  s.ticker = ? AND o.timeframe = ?
    )";

    std::vector<BindValue> bind_params = {
        bind_text(params.symbol),
        bind_text(params.timeframe)};

    if (params.from_time > 0)
    {
        sql += " AND o.open_time >= ?";
        bind_params.push_back(bind_int(params.from_time));
    }
    if (params.to_time > 0)
    {
        sql += " AND o.open_time <= ?";
        bind_params.push_back(bind_int(params.to_time));
    }

    // Fetch window extra rows so the very first output point is valid
    sql += " ORDER BY o.open_time ASC LIMIT " + std::to_string(limit + window);

    const auto rows = db.query(sql, bind_params, "rca_fetch");

    // ── Parse rows ────────────────────────────────────────────────────────────
    struct Candle
    {
        int64_t ts;
        double close;
    };
    std::vector<Candle> candles;
    candles.reserve(rows.size());

    for (const auto &row : rows)
    {
        try
        {
            candles.push_back({std::stoll(row.at("open_time")),
                               std::stod(row.at("close"))});
        }
        catch (...)
        { /* skip malformed rows */
        }
    }

    // ── Build result ──────────────────────────────────────────────────────────
    crow::json::wvalue result;
    result["method"] = name();
    result["symbol"] = params.symbol;
    result["timeframe"] = params.timeframe;
    result["window"] = window;

    std::vector<crow::json::wvalue> data;

    if (static_cast<int>(candles.size()) > window)
    {
        const int end = static_cast<int>(
            std::min(static_cast<size_t>(window + limit), candles.size()));

        for (int i = window; i < end; ++i)
        {
            const double prev = candles[i - window].close;
            if (prev == 0.0)
                continue;

            crow::json::wvalue point;
            point["timestamp"] = candles[i].ts;
            point["close"] = candles[i].close;
            point["roc"] = (candles[i].close - prev) / prev * 100.0;
            data.push_back(std::move(point));
        }
    }

    result["count"] = static_cast<int>(data.size());
    result["data"] = std::move(data);
    return result;
}
