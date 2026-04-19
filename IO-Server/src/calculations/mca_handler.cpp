#include "calculations/mca_handler.hpp"
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

// ── Pearson correlation (population formula) over equal-length vectors ─────
static double pearson(const std::vector<double> &x, const std::vector<double> &y)
{
    const int n = static_cast<int>(x.size());
    if (n < 2)
        return 0.0;

    double sx = 0, sy = 0, sxy = 0, sx2 = 0, sy2 = 0;
    for (int i = 0; i < n; ++i)
    {
        sx += x[i];
        sy += y[i];
        sxy += x[i] * y[i];
        sx2 += x[i] * x[i];
        sy2 += y[i] * y[i];
    }
    const double denom = std::sqrt((n * sx2 - sx * sx) * (n * sy2 - sy * sy));
    return denom == 0.0 ? 0.0 : (n * sxy - sx * sy) / denom;
}

crow::json::wvalue MCAHandler::compute(const CalcParams &params, SQLiteDB &db)
{
    // ── Validate inputs ───────────────────────────────────────────────────────
    if (params.symbol.empty() || params.symbol2.empty())
    {
        crow::json::wvalue err;
        err["error"] = "symbol and symbol2 are both required";
        return err;
    }
    if (params.timeframe.empty())
    {
        crow::json::wvalue err;
        err["error"] = "timeframe is required";
        return err;
    }

    const int window = params.window > 0 ? params.window : 20;
    const int limit = params.limit > 0 ? params.limit : 200;

    // ── Helper: fetch timestamp→close for one ticker ─────────────────────────
    auto fetch = [&](const std::string &ticker) -> std::map<int64_t, double>
    {
        std::string sql = R"(
            SELECT o.open_time, o.close
            FROM   ohlcv o
            JOIN   symbols s ON s.symbol_id = o.symbol_id
            WHERE  s.ticker = ? AND o.timeframe = ?
        )";
        std::vector<BindValue> bp = {bind_text(ticker), bind_text(params.timeframe)};
        if (params.from_time > 0)
        {
            sql += " AND o.open_time >= ?";
            bp.push_back(bind_int(params.from_time));
        }
        if (params.to_time > 0)
        {
            sql += " AND o.open_time <= ?";
            bp.push_back(bind_int(params.to_time));
        }
        sql += " ORDER BY o.open_time ASC LIMIT " + std::to_string(limit + window);

        std::map<int64_t, double> out;
        for (const auto &row : db.query(sql, bp, "mca_fetch"))
        {
            try
            {
                out[std::stoll(row.at("open_time"))] = std::stod(row.at("close"));
            }
            catch (...)
            {
            }
        }
        return out;
    };

    const auto map1 = fetch(params.symbol);
    const auto map2 = fetch(params.symbol2);

    // ── Align on shared timestamps ────────────────────────────────────────────
    struct AlignedBar
    {
        int64_t ts;
        double c1;
        double c2;
    };
    std::vector<AlignedBar> aligned;
    aligned.reserve(std::min(map1.size(), map2.size()));

    for (const auto &[ts, c1] : map1)
    {
        auto it = map2.find(ts);
        if (it != map2.end())
            aligned.push_back({ts, c1, it->second});
    }
    std::sort(aligned.begin(), aligned.end(),
              [](const AlignedBar &a, const AlignedBar &b)
              { return a.ts < b.ts; });

    // ── Build result ──────────────────────────────────────────────────────────
    crow::json::wvalue result;
    result["method"] = name();
    result["symbol"] = params.symbol;
    result["symbol2"] = params.symbol2;
    result["timeframe"] = params.timeframe;
    result["window"] = window;

    std::vector<crow::json::wvalue> data;

    if (static_cast<int>(aligned.size()) >= window)
    {
        for (int i = window - 1, output_count = 0;
             i < static_cast<int>(aligned.size()) && output_count < limit;
             ++i)
        {
            std::vector<double> x, y;
            x.reserve(window);
            y.reserve(window);
            for (int j = i - window + 1; j <= i; ++j)
            {
                x.push_back(aligned[j].c1);
                y.push_back(aligned[j].c2);
            }
            crow::json::wvalue point;
            point["timestamp"] = aligned[i].ts;
            point["correlation"] = pearson(x, y);
            data.push_back(std::move(point));
            ++output_count;
        }
    }

    result["count"] = static_cast<int>(data.size());
    result["data"] = std::move(data);
    return result;
}
