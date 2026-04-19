#include "api_router.hpp"
#include <nlohmann/json.hpp>
#include <set>
#include <sstream>
#include <iomanip>

using json = nlohmann::json;

APIRouter::APIRouter(std::shared_ptr<SQLiteDB> db,
                     std::shared_ptr<CalculationRegistry> calc_registry)
    : db_(std::move(db)), calc_registry_(std::move(calc_registry)) {}

APIRouter::QueryParams APIRouter::parse_params(const crow::request &req)
{
    QueryParams params;

    auto symbol = req.url_params.get("symbol");
    params.symbol = symbol ? symbol : "";

    auto timeframe = req.url_params.get("timeframe");
    params.timeframe = timeframe ? timeframe : "";

    auto from = req.url_params.get("from");
    try
    {
        params.from_time = from ? std::stoll(from) : 0;
    }
    catch (...)
    {
        params.from_time = 0;
    }

    auto to = req.url_params.get("to");
    try
    {
        params.to_time = to ? std::stoll(to) : 0;
    }
    catch (...)
    {
        params.to_time = 0;
    }

    auto limit = req.url_params.get("limit");
    try
    {
        params.limit = limit ? std::stoi(limit) : 100;
    }
    catch (...)
    {
        params.limit = 100;
    }

    auto offset = req.url_params.get("offset");
    try
    {
        params.offset = offset ? std::stoi(offset) : 0;
    }
    catch (...)
    {
        params.offset = 0;
    }

    auto strategy = req.url_params.get("strategy");
    params.strategy = strategy ? strategy : "";

    auto status = req.url_params.get("status");
    params.status = status ? status : "";

    auto direction = req.url_params.get("direction");
    params.direction = direction ? direction : "";

    auto order_by = req.url_params.get("order_by");
    params.order_by = order_by ? order_by : "";

    auto order_dir = req.url_params.get("order_dir");
    params.order_dir = order_dir ? order_dir : "ASC";

    return params;
}

void APIRouter::setup_routes(crow::SimpleApp &app)
{
    // Symbols endpoints
    CROW_ROUTE(app, "/api/symbols").methods("GET"_method)([this](const crow::request &req)
                                                          { return get_symbols(req); });

    CROW_ROUTE(app, "/api/symbols/<string>").methods("GET"_method)([this](const crow::request &req, const std::string &ticker)
                                                                   { return get_symbol(req, ticker); });

    // OHLCV endpoints
    CROW_ROUTE(app, "/api/ohlcv").methods("GET"_method)([this](const crow::request &req)
                                                        { return get_ohlcv(req); });

    CROW_ROUTE(app, "/api/ohlcv/latest").methods("GET"_method)([this](const crow::request &req)
                                                               { return get_latest_ohlcv(req); });

    // Orderbook endpoints
    CROW_ROUTE(app, "/api/orderbook").methods("GET"_method)([this](const crow::request &req)
                                                            { return get_orderbook(req); });

    CROW_ROUTE(app, "/api/orderbook/snapshot/<int>").methods("GET"_method)([this](const crow::request &req, int64_t snapshot_time)
                                                                           { return get_orderbook_snapshot(req, snapshot_time); });

    // Strategy endpoints
    CROW_ROUTE(app, "/api/strategies").methods("GET"_method)([this](const crow::request &req)
                                                             { return get_strategies(req); });

    CROW_ROUTE(app, "/api/strategies").methods("POST"_method)([this](const crow::request &req)
                                                              { return create_strategy(req); });

    CROW_ROUTE(app, "/api/strategies/<int>").methods("GET"_method)([this](const crow::request &req, int strategy_id)
                                                                   { return get_strategy(req, strategy_id); });

    CROW_ROUTE(app, "/api/strategies/<int>").methods("PUT"_method)([this](const crow::request &req, int strategy_id)
                                                                   { return update_strategy(req, strategy_id); });

    // Orders endpoints
    CROW_ROUTE(app, "/api/orders").methods("GET"_method)([this](const crow::request &req)
                                                         { return get_orders(req); });

    CROW_ROUTE(app, "/api/orders").methods("POST"_method)([this](const crow::request &req)
                                                          { return create_order(req); });

    CROW_ROUTE(app, "/api/orders/<int>").methods("GET"_method)([this](const crow::request &req, int order_id)
                                                               { return get_order(req, order_id); });

    CROW_ROUTE(app, "/api/orders/client/<string>").methods("GET"_method)([this](const crow::request &req, const std::string &client_id)
                                                                         { return get_orders_by_client_id(req, client_id); });

    CROW_ROUTE(app, "/api/orders/<int>/cancel").methods("POST"_method)([this](const crow::request &req, int order_id)
                                                                       { return cancel_order(req, order_id); });

    // Signals endpoints
    CROW_ROUTE(app, "/api/signals").methods("GET"_method)([this](const crow::request &req)
                                                          { return get_signals(req); });

    CROW_ROUTE(app, "/api/signals").methods("POST"_method)([this](const crow::request &req)
                                                           { return create_signal(req); });

    CROW_ROUTE(app, "/api/signals/<int>").methods("GET"_method)([this](const crow::request &req, int signal_id)
                                                                { return get_signal(req, signal_id); });

    // Positions endpoints
    CROW_ROUTE(app, "/api/positions").methods("GET"_method)([this](const crow::request &req)
                                                            { return get_positions(req); });

    CROW_ROUTE(app, "/api/positions/<int>").methods("GET"_method)([this](const crow::request &req, int position_id)
                                                                  { return get_position(req, position_id); });

    CROW_ROUTE(app, "/api/strategies/<int>/positions").methods("GET"_method)([this](const crow::request &req, int strategy_id)
                                                                             { return get_positions_by_strategy(req, strategy_id); });

    // Performance endpoints
    CROW_ROUTE(app, "/api/performance").methods("GET"_method)([this](const crow::request &req)
                                                              { return get_performance(req); });

    CROW_ROUTE(app, "/api/strategies/<int>/performance").methods("GET"_method)([this](const crow::request &req, int strategy_id)
                                                                               { return get_strategy_performance(req, strategy_id); });

    // ── Calculation endpoints (only wired if a registry was provided) ─────────
    if (calc_registry_)
    {
        CROW_ROUTE(app, "/api/calc").methods("GET"_method)([this](const crow::request &req)
                                                           { return list_calculations(req); });

        CROW_ROUTE(app, "/api/calc/<string>").methods("GET"_method)([this](const crow::request &req, const std::string &method)
                                                                    { return run_calculation(req, method); });
    }
}

// ── Calculation helpers ───────────────────────────────────────────────────────

CalcParams APIRouter::parse_calc_params(const crow::request &req)
{
    CalcParams p;

    if (auto v = req.url_params.get("symbol"))
        p.symbol = v;
    if (auto v = req.url_params.get("symbol2"))
        p.symbol2 = v;
    if (auto v = req.url_params.get("timeframe"))
        p.timeframe = v;
    if (auto v = req.url_params.get("from"))
        try
        {
            p.from_time = std::stoll(v);
        }
        catch (...)
        {
        }
    if (auto v = req.url_params.get("to"))
        try
        {
            p.to_time = std::stoll(v);
        }
        catch (...)
        {
        }
    if (auto v = req.url_params.get("window"))
        try
        {
            p.window = std::stoi(v);
        }
        catch (...)
        {
        }
    if (auto v = req.url_params.get("limit"))
        try
        {
            p.limit = std::stoi(v);
        }
        catch (...)
        {
        }

    // Forward any unknown query parameters into CalcParams::extra so that
    // specialised handlers can use them without touching the base struct.
    for (const auto &key : req.url_params.keys())
    {
        static const std::set<std::string> known = {
            "symbol", "symbol2", "timeframe", "from", "to", "window", "limit"};
        if (!known.count(key))
            p.extra[key] = req.url_params.get(key);
    }
    return p;
}

crow::response APIRouter::list_calculations(const crow::request & /*req*/)
{
    return crow::response(calc_registry_->list_json());
}

crow::response APIRouter::run_calculation(const crow::request &req,
                                          const std::string &method)
{
    auto handler = calc_registry_->get(method);
    if (!handler)
        return not_found_response("Calculation method '" + method + "'");

    try
    {
        CalcParams p = parse_calc_params(req);
        auto result = handler->compute(p, *db_);

        // Surface computation-level errors as HTTP 400
        if (result.t() == crow::json::type::Object)
        {
            auto err = result["error"];
            if (err.t() == crow::json::type::String)
            {
                // Use public API to obtain string value. Avoid accessing private members.
                std::string msg;
                try
                {
                    msg = err.dump(); // dump() returns JSON; strip quotes if necessary
                }
                catch (...)
                {
                    msg.clear();
                }
                if (msg.size() >= 2 && msg.front() == '"' && msg.back() == '"')
                    msg = msg.substr(1, msg.size() - 2);

                return error_response(msg.empty() ? std::string("error") : msg, 400);
            }
        }
        return crow::response(std::move(result));
    }
    catch (const std::exception &e)
    {
        return error_response(e.what(), 500);
    }
}

std::string APIRouter::build_date_range_filter(const std::string &field, int64_t from, int64_t to)
{
    std::string filter;
    if (from > 0 && to > 0)
    {
        filter = " AND " + field + " BETWEEN " + std::to_string(from) + " AND " + std::to_string(to);
    }
    else if (from > 0)
    {
        filter = " AND " + field + " >= " + std::to_string(from);
    }
    else if (to > 0)
    {
        filter = " AND " + field + " <= " + std::to_string(to);
    }
    return filter;
}

std::string APIRouter::build_order_by_clause(const std::string &default_field,
                                             const std::string &direction)
{
    return " ORDER BY " + default_field + " " + direction;
}

crow::json::wvalue APIRouter::row_to_json(const Row &row)
{
    crow::json::wvalue json;
    for (const auto &[key, value] : row)
    {
        // Try to parse numbers
        try
        {
            if (!value.empty())
            {
                // Check if it's a number
                char *endptr;
                long long int_val = std::strtoll(value.c_str(), &endptr, 10);
                if (*endptr == '\0')
                {
                    json[key] = int_val;
                    continue;
                }

                double double_val = std::strtod(value.c_str(), &endptr);
                if (*endptr == '\0')
                {
                    json[key] = double_val;
                    continue;
                }
            }
        }
        catch (...)
        {
            // Fall back to string
        }
        json[key] = value;
    }
    return json;
}

crow::json::wvalue APIRouter::rows_to_json(const RowSet &rows)
{
    crow::json::wvalue json;
    std::vector<crow::json::wvalue> items;

    for (const auto &row : rows)
    {
        items.push_back(row_to_json(row));
    }

    json["data"] = std::move(items);
    json["count"] = rows.size();
    json["success"] = true;

    return json;
}

crow::response APIRouter::error_response(const std::string &message, int code)
{
    crow::json::wvalue error;
    error["success"] = false;
    error["error"] = message;
    error["code"] = code;
    return crow::response(code, error);
}

crow::response APIRouter::not_found_response(const std::string &resource)
{
    return error_response(resource + " not found", 404);
}

// Symbols endpoints
crow::response APIRouter::get_symbols(const crow::request &req)
{
    try
    {
        auto params = parse_params(req);

        std::string sql = "SELECT * FROM symbols WHERE 1=1";
        std::vector<BindValue> binds;

        if (!params.symbol.empty())
        {
            sql += " AND ticker LIKE ?";
            binds.push_back(bind_text("%" + params.symbol + "%"));
        }

        sql += " ORDER BY ticker ASC LIMIT ? OFFSET ?";
        binds.push_back(bind_int(params.limit));
        binds.push_back(bind_int(params.offset));

        auto results = db_->query(sql, binds, "get_symbols");
        return crow::response(rows_to_json(results));
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::get_symbol(const crow::request &req, const std::string &ticker)
{
    try
    {
        std::string sql = "SELECT * FROM symbols WHERE ticker = ?";
        auto results = db_->query(sql, {bind_text(ticker)}, "get_symbol");

        if (results.empty())
        {
            return not_found_response("Symbol " + ticker);
        }

        crow::json::wvalue response;
        response["data"] = row_to_json(results[0]);
        response["success"] = true;
        return crow::response(response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

// OHLCV endpoints
crow::response APIRouter::get_ohlcv(const crow::request &req)
{
    try
    {
        auto params = parse_params(req);

        if (params.symbol.empty())
        {
            return error_response("symbol parameter is required");
        }

        // First get symbol_id
        std::string sym_sql = "SELECT symbol_id FROM symbols WHERE ticker = ?";
        auto sym_result = db_->query(sym_sql, {bind_text(params.symbol)}, "get_symbol_id");

        if (sym_result.empty())
        {
            return error_response("Symbol not found: " + params.symbol);
        }

        int symbol_id = std::stoi(sym_result[0].at("symbol_id"));

        std::string sql = "SELECT * FROM ohlcv WHERE symbol_id = ?";
        std::vector<BindValue> binds;
        binds.push_back(bind_int(symbol_id));

        if (!params.timeframe.empty())
        {
            sql += " AND timeframe = ?";
            binds.push_back(bind_text(params.timeframe));
        }

        sql += build_date_range_filter("open_time", params.from_time, params.to_time);
        sql += build_order_by_clause("open_time", params.order_dir);
        sql += " LIMIT ? OFFSET ?";
        binds.push_back(bind_int(params.limit));
        binds.push_back(bind_int(params.offset));

        auto results = db_->query(sql, binds, "get_ohlcv");

        // Add metadata
        crow::json::wvalue response = rows_to_json(results);
        response["symbol"] = params.symbol;
        response["timeframe"] = params.timeframe.empty() ? "all" : params.timeframe;

        return crow::response(response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::get_latest_ohlcv(const crow::request &req)
{
    try
    {
        auto params = parse_params(req);

        if (params.symbol.empty())
        {
            return error_response("symbol parameter is required");
        }

        std::string sql = R"(
            SELECT o.* FROM ohlcv o
            JOIN symbols s ON o.symbol_id = s.symbol_id
            WHERE s.ticker = ?
        )";
        std::vector<BindValue> binds;
        binds.push_back(bind_text(params.symbol));

        if (!params.timeframe.empty())
        {
            sql += " AND o.timeframe = ?";
            binds.push_back(bind_text(params.timeframe));
        }

        sql += " ORDER BY o.open_time DESC LIMIT 1";

        auto results = db_->query(sql, binds, "get_latest_ohlcv");

        if (results.empty())
        {
            return not_found_response("OHLCV data for " + params.symbol);
        }

        crow::json::wvalue response;
        response["data"] = row_to_json(results[0]);
        response["success"] = true;
        return crow::response(response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

// Orderbook endpoints
crow::response APIRouter::get_orderbook(const crow::request &req)
{
    try
    {
        auto params = parse_params(req);

        if (params.symbol.empty())
        {
            return error_response("symbol parameter is required");
        }

        std::string sql = R"(
            SELECT ob.* FROM orderbook ob
            JOIN symbols s ON ob.symbol_id = s.symbol_id
            WHERE s.ticker = ?
        )";
        std::vector<BindValue> binds;
        binds.push_back(bind_text(params.symbol));

        sql += build_date_range_filter("ob.snapshot_time", params.from_time, params.to_time);
        sql += " ORDER BY ob.snapshot_time DESC, ob.level ASC";
        sql += " LIMIT ? OFFSET ?";
        binds.push_back(bind_int(params.limit));
        binds.push_back(bind_int(params.offset));

        auto results = db_->query(sql, binds, "get_orderbook");

        // Group by snapshot_time for better readability
        std::map<int64_t, std::vector<Row>> grouped;
        for (const auto &row : results)
        {
            int64_t snapshot = std::stoll(row.at("snapshot_time"));
            grouped[snapshot].push_back(row);
        }

        crow::json::wvalue response;
        response["success"] = true;
        response["symbol"] = params.symbol;

        std::vector<crow::json::wvalue> snapshots;
        for (const auto &[time, orders] : grouped)
        {
            crow::json::wvalue snapshot;
            snapshot["snapshot_time"] = time;

            std::vector<crow::json::wvalue> bids;
            std::vector<crow::json::wvalue> asks;

            for (const auto &order : orders)
            {
                if (order.at("side") == "BID")
                {
                    bids.push_back(row_to_json(order));
                }
                else
                {
                    asks.push_back(row_to_json(order));
                }
            }

            snapshot["bids"] = std::move(bids);
            snapshot["asks"] = std::move(asks);
            snapshot["bid_count"] = bids.size();
            snapshot["ask_count"] = asks.size();

            snapshots.push_back(std::move(snapshot));
        }

        response["data"] = std::move(snapshots);
        response["count"] = snapshots.size();

        return crow::response(response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::get_orderbook_snapshot(const crow::request &req, int64_t snapshot_time)
{
    try
    {
        auto params = parse_params(req);

        if (params.symbol.empty())
        {
            return error_response("symbol parameter is required");
        }

        std::string sql = R"(
            SELECT ob.* FROM orderbook ob
            JOIN symbols s ON ob.symbol_id = s.symbol_id
            WHERE s.ticker = ? AND ob.snapshot_time = ?
            ORDER BY ob.level ASC
        )";

        auto results = db_->query(sql, {bind_text(params.symbol), bind_int(snapshot_time)}, "get_orderbook_snapshot");

        if (results.empty())
        {
            return not_found_response("Orderbook snapshot");
        }

        crow::json::wvalue response;
        response["success"] = true;
        response["snapshot_time"] = snapshot_time;
        response["symbol"] = params.symbol;

        std::vector<crow::json::wvalue> bids;
        std::vector<crow::json::wvalue> asks;

        for (const auto &order : results)
        {
            if (order.at("side") == "BID")
            {
                bids.push_back(row_to_json(order));
            }
            else
            {
                asks.push_back(row_to_json(order));
            }
        }

        response["bids"] = std::move(bids);
        response["asks"] = std::move(asks);
        response["bid_count"] = bids.size();
        response["ask_count"] = asks.size();

        return crow::response(response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

// Strategies endpoints
crow::response APIRouter::get_strategies(const crow::request &req)
{
    try
    {
        auto params = parse_params(req);

        std::string sql = "SELECT * FROM strategy WHERE 1=1";
        std::vector<BindValue> binds;

        if (!params.status.empty())
        {
            sql += " AND status = ?";
            binds.push_back(bind_text(params.status));
        }

        if (!params.strategy.empty())
        {
            sql += " AND name LIKE ?";
            binds.push_back(bind_text("%" + params.strategy + "%"));
        }

        sql += " ORDER BY name ASC LIMIT ? OFFSET ?";
        binds.push_back(bind_int(params.limit));
        binds.push_back(bind_int(params.offset));

        auto results = db_->query(sql, binds, "get_strategies");
        return crow::response(rows_to_json(results));
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::get_strategy(const crow::request &req, int strategy_id)
{
    try
    {
        std::string sql = "SELECT * FROM strategy WHERE strategy_id = ?";
        auto results = db_->query(sql, {bind_int(strategy_id)}, "get_strategy");

        if (results.empty())
        {
            return not_found_response("Strategy");
        }

        crow::json::wvalue response;
        response["data"] = row_to_json(results[0]);
        response["success"] = true;
        return crow::response(response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::create_strategy(const crow::request &req)
{
    try
    {
        auto body = json::parse(req.body);

        std::string sql = R"(
            INSERT INTO strategy (
                name, version, description, author, status, mode,
                capital_alloc, max_position, max_drawdown, risk_per_trade,
                params_json, started_at
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        )";

        std::vector<BindValue> binds;
        binds.push_back(bind_text(body.value("name", "")));
        binds.push_back(bind_text(body.value("version", "1.0.0")));
        binds.push_back(bind_text(body.value("description", "")));
        binds.push_back(bind_text(body.value("author", "")));
        binds.push_back(bind_text(body.value("status", "DRAFT")));
        binds.push_back(bind_text(body.value("mode", "PAPER")));
        binds.push_back(bind_real(body.value("capital_alloc", 0.0)));
        binds.push_back(bind_real(body.value("max_position", 0.0)));
        binds.push_back(bind_real(body.value("max_drawdown", 0.05)));
        binds.push_back(bind_real(body.value("risk_per_trade", 0.01)));
        binds.push_back(bind_text(body.value("params_json", "{}")));

        // Started at timestamp if provided
        if (body.contains("started_at"))
        {
            binds.push_back(bind_int(body["started_at"]));
        }
        else
        {
            binds.push_back(bind_null());
        }

        db_->exec(sql, binds, "create_strategy");

        int64_t new_id = db_->last_insert_rowid();

        crow::json::wvalue response;
        response["success"] = true;
        response["strategy_id"] = new_id;
        response["message"] = "Strategy created successfully";

        return crow::response(201, response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::update_strategy(const crow::request &req, int strategy_id)
{
    try
    {
        // First check if strategy exists
        auto check = db_->query(
            "SELECT strategy_id FROM strategy WHERE strategy_id = ?",
            {bind_int(strategy_id)},
            "check_strategy");

        if (check.empty())
        {
            return not_found_response("Strategy");
        }

        auto body = json::parse(req.body);

        std::string sql = "UPDATE strategy SET updated_at = SYSUTCDATETIME()";
        std::vector<BindValue> binds;

        // Build dynamic update based on provided fields
        if (body.contains("name"))
        {
            sql += ", name = ?";
            binds.push_back(bind_text(body["name"]));
        }
        if (body.contains("version"))
        {
            sql += ", version = ?";
            binds.push_back(bind_text(body["version"]));
        }
        if (body.contains("status"))
        {
            sql += ", status = ?";
            binds.push_back(bind_text(body["status"]));
        }
        if (body.contains("mode"))
        {
            sql += ", mode = ?";
            binds.push_back(bind_text(body["mode"]));
        }
        if (body.contains("capital_alloc"))
        {
            sql += ", capital_alloc = ?";
            binds.push_back(bind_real(body["capital_alloc"]));
        }
        if (body.contains("max_position"))
        {
            sql += ", max_position = ?";
            binds.push_back(bind_real(body["max_position"]));
        }
        if (body.contains("params_json"))
        {
            sql += ", params_json = ?";
            binds.push_back(bind_text(body["params_json"].dump()));
        }

        sql += " WHERE strategy_id = ?";
        binds.push_back(bind_int(strategy_id));

        db_->exec(sql, binds, "update_strategy");

        crow::json::wvalue response;
        response["success"] = true;
        response["message"] = "Strategy updated successfully";

        return crow::response(response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

// Orders endpoints
crow::response APIRouter::get_orders(const crow::request &req)
{
    try
    {
        auto params = parse_params(req);

        std::string sql = R"(
            SELECT o.*, s.ticker as symbol_ticker, st.name as strategy_name
            FROM orders o
            JOIN symbols s ON o.symbol_id = s.symbol_id
            LEFT JOIN strategy st ON o.strategy_id = st.strategy_id
            WHERE 1=1
        )";
        std::vector<BindValue> binds;

        if (!params.symbol.empty())
        {
            sql += " AND s.ticker = ?";
            binds.push_back(bind_text(params.symbol));
        }

        if (!params.status.empty())
        {
            sql += " AND o.status = ?";
            binds.push_back(bind_text(params.status));
        }

        if (!params.strategy.empty() && params.strategy != "0")
        {
            sql += " AND o.strategy_id = ?";
            binds.push_back(bind_int(std::stoi(params.strategy)));
        }

        // Date range on submitted_at
        sql += build_date_range_filter("o.submitted_at", params.from_time, params.to_time);

        sql += build_order_by_clause("o.submitted_at", "DESC");
        sql += " LIMIT ? OFFSET ?";
        binds.push_back(bind_int(params.limit));
        binds.push_back(bind_int(params.offset));

        auto results = db_->query(sql, binds, "get_orders");
        return crow::response(rows_to_json(results));
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::get_order(const crow::request &req, int order_id)
{
    try
    {
        std::string sql = R"(
            SELECT o.*, s.ticker as symbol_ticker, st.name as strategy_name
            FROM orders o
            JOIN symbols s ON o.symbol_id = s.symbol_id
            LEFT JOIN strategy st ON o.strategy_id = st.strategy_id
            WHERE o.order_id = ?
        )";

        auto results = db_->query(sql, {bind_int(order_id)}, "get_order");

        if (results.empty())
        {
            return not_found_response("Order");
        }

        crow::json::wvalue response;
        response["data"] = row_to_json(results[0]);
        response["success"] = true;
        return crow::response(response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::get_orders_by_client_id(const crow::request &req, const std::string &client_id)
{
    try
    {
        std::string sql = R"(
            SELECT o.*, s.ticker as symbol_ticker, st.name as strategy_name
            FROM orders o
            JOIN symbols s ON o.symbol_id = s.symbol_id
            LEFT JOIN strategy st ON o.strategy_id = st.strategy_id
            WHERE o.client_order_id = ?
        )";

        auto results = db_->query(sql, {bind_text(client_id)}, "get_orders_by_client_id");

        if (results.empty())
        {
            return not_found_response("Order with client_id: " + client_id);
        }

        return crow::response(rows_to_json(results));
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::create_order(const crow::request &req)
{
    try
    {
        auto body = json::parse(req.body);

        // Validate required fields
        std::vector<std::string> required = {"client_order_id", "symbol", "order_type",
                                             "side", "quantity"};
        for (const auto &field : required)
        {
            if (!body.contains(field))
            {
                return error_response("Missing required field: " + field);
            }
        }

        // Get symbol_id
        auto symbol_result = db_->query(
            "SELECT symbol_id FROM symbols WHERE ticker = ?",
            {bind_text(body["symbol"])},
            "get_symbol_id");

        if (symbol_result.empty())
        {
            return error_response("Symbol not found: " + body["symbol"].get<std::string>());
        }

        int symbol_id = std::stoi(symbol_result[0].at("symbol_id"));

        // Optional strategy_id
        int strategy_id = 0;
        if (body.contains("strategy"))
        {
            auto strat_result = db_->query(
                "SELECT strategy_id FROM strategy WHERE name = ?",
                {bind_text(body["strategy"])},
                "get_strategy_id");
            if (!strat_result.empty())
            {
                strategy_id = std::stoi(strat_result[0].at("strategy_id"));
            }
        }

        // Insert order
        std::string sql = R"(
            INSERT INTO orders (
                client_order_id, symbol_id, strategy_id, order_type, side,
                price, stop_price, quantity, time_in_force, expire_time,
                submitted_at, status
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        )";

        std::vector<BindValue> binds;
        binds.push_back(bind_text(body["client_order_id"]));
        binds.push_back(bind_int(symbol_id));

        if (strategy_id > 0)
        {
            binds.push_back(bind_int(strategy_id));
        }
        else
        {
            binds.push_back(bind_null());
        }

        binds.push_back(bind_text(body["order_type"]));
        binds.push_back(bind_text(body["side"]));

        // Optional price fields
        if (body.contains("price"))
        {
            binds.push_back(bind_real(body["price"]));
        }
        else
        {
            binds.push_back(bind_null());
        }

        if (body.contains("stop_price"))
        {
            binds.push_back(bind_real(body["stop_price"]));
        }
        else
        {
            binds.push_back(bind_null());
        }

        binds.push_back(bind_real(body["quantity"]));
        binds.push_back(bind_text(body.value("time_in_force", "GTC")));

        if (body.contains("expire_time"))
        {
            binds.push_back(bind_int(body["expire_time"]));
        }
        else
        {
            binds.push_back(bind_null());
        }

        // Submitted time (current time if not provided)
        int64_t now = std::time(nullptr);
        binds.push_back(bind_int(body.value("submitted_at", now)));

        // Status
        binds.push_back(bind_text("NEW"));

        db_->exec(sql, binds, "create_order");

        int64_t new_order_id = db_->last_insert_rowid();

        crow::json::wvalue response;
        response["success"] = true;
        response["order_id"] = new_order_id;
        response["client_order_id"] = body["client_order_id"].dump();
        response["message"] = "Order created successfully";

        return crow::response(201, response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::cancel_order(const crow::request &req, int order_id)
{
    try
    {
        // Check if order exists and can be cancelled
        auto check = db_->query(
            "SELECT status FROM orders WHERE order_id = ?",
            {bind_int(order_id)},
            "check_order");

        if (check.empty())
        {
            return not_found_response("Order");
        }

        std::string status = check[0].at("status");
        if (status != "NEW" && status != "PARTIALLY_FILLED")
        {
            return error_response("Order cannot be cancelled in status: " + status);
        }

        // Update order
        int64_t now = std::time(nullptr);
        db_->exec(
            "UPDATE orders SET status = 'CANCELLED', cancelled_at = ?, updated_at = SYSUTCDATETIME() WHERE order_id = ?",
            {bind_int(now), bind_int(order_id)},
            "cancel_order");

        crow::json::wvalue response;
        response["success"] = true;
        response["message"] = "Order cancelled successfully";

        return crow::response(response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

// Signals endpoints
crow::response APIRouter::get_signals(const crow::request &req)
{
    try
    {
        auto params = parse_params(req);

        std::string sql = R"(
            SELECT sig.*, s.ticker as symbol_ticker, st.name as strategy_name
            FROM signal sig
            JOIN symbols s ON sig.symbol_id = s.symbol_id
            JOIN strategy st ON sig.strategy_id = st.strategy_id
            WHERE 1=1
        )";
        std::vector<BindValue> binds;

        if (!params.symbol.empty())
        {
            sql += " AND s.ticker = ?";
            binds.push_back(bind_text(params.symbol));
        }

        if (!params.strategy.empty())
        {
            sql += " AND st.name = ?";
            binds.push_back(bind_text(params.strategy));
        }

        if (!params.status.empty())
        {
            sql += " AND sig.status = ?";
            binds.push_back(bind_text(params.status));
        }

        if (!params.direction.empty())
        {
            sql += " AND sig.direction = ?";
            binds.push_back(bind_text(params.direction));
        }

        sql += build_date_range_filter("sig.signal_time", params.from_time, params.to_time);
        sql += build_order_by_clause("sig.signal_time", "DESC");
        sql += " LIMIT ? OFFSET ?";
        binds.push_back(bind_int(params.limit));
        binds.push_back(bind_int(params.offset));

        auto results = db_->query(sql, binds, "get_signals");
        return crow::response(rows_to_json(results));
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::get_signal(const crow::request &req, int signal_id)
{
    try
    {
        std::string sql = R"(
            SELECT sig.*, s.ticker as symbol_ticker, st.name as strategy_name
            FROM signal sig
            JOIN symbols s ON sig.symbol_id = s.symbol_id
            JOIN strategy st ON sig.strategy_id = st.strategy_id
            WHERE sig.signal_id = ?
        )";

        auto results = db_->query(sql, {bind_int(signal_id)}, "get_signal");

        if (results.empty())
        {
            return not_found_response("Signal");
        }

        crow::json::wvalue response;
        response["data"] = row_to_json(results[0]);
        response["success"] = true;
        return crow::response(response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::create_signal(const crow::request &req)
{
    try
    {
        auto body = json::parse(req.body);

        // Validate required fields
        std::vector<std::string> required = {"strategy", "symbol", "signal_type",
                                             "direction", "signal_time"};
        for (const auto &field : required)
        {
            if (!body.contains(field))
            {
                return error_response("Missing required field: " + field);
            }
        }

        // Get strategy_id
        auto strategy_result = db_->query(
            "SELECT strategy_id FROM strategy WHERE name = ?",
            {bind_text(body["strategy"])},
            "get_strategy_id");

        if (strategy_result.empty())
        {
            return error_response("Strategy not found: " + body["strategy"].get<std::string>());
        }

        int strategy_id = std::stoi(strategy_result[0].at("strategy_id"));

        // Get symbol_id
        auto symbol_result = db_->query(
            "SELECT symbol_id FROM symbols WHERE ticker = ?",
            {bind_text(body["symbol"])},
            "get_symbol_id");

        if (symbol_result.empty())
        {
            return error_response("Symbol not found: " + body["symbol"].get<std::string>());
        }

        int symbol_id = std::stoi(symbol_result[0].at("symbol_id"));

        // Insert signal
        std::string sql = R"(
            INSERT INTO signal (
                strategy_id, symbol_id, signal_time, signal_type, direction,
                strength, price_target, stop_loss, take_profit, timeframe,
                indicator_json, status, expiry_time, notes
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        )";

        std::vector<BindValue> binds;
        binds.push_back(bind_int(strategy_id));
        binds.push_back(bind_int(symbol_id));
        binds.push_back(bind_int(body["signal_time"]));
        binds.push_back(bind_text(body["signal_type"]));
        binds.push_back(bind_text(body["direction"]));

        // Optional fields
        binds.push_back(bind_real(body.value("strength", 1.0)));

        if (body.contains("price_target"))
        {
            binds.push_back(bind_real(body["price_target"]));
        }
        else
        {
            binds.push_back(bind_null());
        }

        if (body.contains("stop_loss"))
        {
            binds.push_back(bind_real(body["stop_loss"]));
        }
        else
        {
            binds.push_back(bind_null());
        }

        if (body.contains("take_profit"))
        {
            binds.push_back(bind_real(body["take_profit"]));
        }
        else
        {
            binds.push_back(bind_null());
        }

        if (body.contains("timeframe"))
        {
            binds.push_back(bind_text(body["timeframe"]));
        }
        else
        {
            binds.push_back(bind_null());
        }

        if (body.contains("indicator_json"))
        {
            binds.push_back(bind_text(body["indicator_json"].dump()));
        }
        else
        {
            binds.push_back(bind_null());
        }

        binds.push_back(bind_text(body.value("status", "PENDING")));

        if (body.contains("expiry_time"))
        {
            binds.push_back(bind_int(body["expiry_time"]));
        }
        else
        {
            binds.push_back(bind_null());
        }

        if (body.contains("notes"))
        {
            binds.push_back(bind_text(body["notes"]));
        }
        else
        {
            binds.push_back(bind_null());
        }

        db_->exec(sql, binds, "create_signal");

        int64_t new_signal_id = db_->last_insert_rowid();

        crow::json::wvalue response;
        response["success"] = true;
        response["signal_id"] = new_signal_id;
        response["message"] = "Signal created successfully";

        return crow::response(201, response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

// Positions endpoints
crow::response APIRouter::get_positions(const crow::request &req)
{
    try
    {
        auto params = parse_params(req);

        std::string sql = R"(
            SELECT p.*, s.ticker as symbol_ticker, st.name as strategy_name
            FROM positions p
            JOIN symbols s ON p.symbol_id = s.symbol_id
            JOIN strategy st ON p.strategy_id = st.strategy_id
            WHERE 1=1
        )";
        std::vector<BindValue> binds;

        if (!params.symbol.empty())
        {
            sql += " AND s.ticker = ?";
            binds.push_back(bind_text(params.symbol));
        }

        if (!params.strategy.empty())
        {
            sql += " AND st.name = ?";
            binds.push_back(bind_text(params.strategy));
        }

        if (!params.direction.empty())
        {
            sql += " AND p.direction = ?";
            binds.push_back(bind_text(params.direction));
        }

        sql += build_order_by_clause("p.opened_at", "DESC");
        sql += " LIMIT ? OFFSET ?";
        binds.push_back(bind_int(params.limit));
        binds.push_back(bind_int(params.offset));

        auto results = db_->query(sql, binds, "get_positions");
        return crow::response(rows_to_json(results));
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::get_position(const crow::request &req, int position_id)
{
    try
    {
        std::string sql = R"(
            SELECT p.*, s.ticker as symbol_ticker, st.name as strategy_name
            FROM positions p
            JOIN symbols s ON p.symbol_id = s.symbol_id
            JOIN strategy st ON p.strategy_id = st.strategy_id
            WHERE p.position_id = ?
        )";

        auto results = db_->query(sql, {bind_int(position_id)}, "get_position");

        if (results.empty())
        {
            return not_found_response("Position");
        }

        crow::json::wvalue response;
        response["data"] = row_to_json(results[0]);
        response["success"] = true;
        return crow::response(response);
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::get_positions_by_strategy(const crow::request &req, int strategy_id)
{
    try
    {
        std::string sql = R"(
            SELECT p.*, s.ticker as symbol_ticker, st.name as strategy_name
            FROM positions p
            JOIN symbols s ON p.symbol_id = s.symbol_id
            JOIN strategy st ON p.strategy_id = st.strategy_id
            WHERE p.strategy_id = ?
            ORDER BY p.opened_at DESC
        )";

        auto results = db_->query(sql, {bind_int(strategy_id)}, "get_positions_by_strategy");
        return crow::response(rows_to_json(results));
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

// Performance endpoints
crow::response APIRouter::get_performance(const crow::request &req)
{
    try
    {
        auto params = parse_params(req);

        std::string sql = R"(
            SELECT pm.*, st.name as strategy_name
            FROM performance_metrics pm
            JOIN strategy st ON pm.strategy_id = st.strategy_id
            WHERE 1=1
        )";
        std::vector<BindValue> binds;

        if (!params.strategy.empty())
        {
            sql += " AND st.name = ?";
            binds.push_back(bind_text(params.strategy));
        }

        // Date range on date field
        if (params.from_time > 0)
        {
            sql += " AND pm.date >= date(?, 'unixepoch')";
            binds.push_back(bind_int(params.from_time));
        }
        if (params.to_time > 0)
        {
            sql += " AND pm.date <= date(?, 'unixepoch')";
            binds.push_back(bind_int(params.to_time));
        }

        sql += build_order_by_clause("pm.date", "DESC");
        sql += " LIMIT ? OFFSET ?";
        binds.push_back(bind_int(params.limit));
        binds.push_back(bind_int(params.offset));

        auto results = db_->query(sql, binds, "get_performance");
        return crow::response(rows_to_json(results));
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}

crow::response APIRouter::get_strategy_performance(const crow::request &req, int strategy_id)
{
    try
    {
        auto params = parse_params(req);

        std::string sql = R"(
            SELECT pm.*, st.name as strategy_name
            FROM performance_metrics pm
            JOIN strategy st ON pm.strategy_id = st.strategy_id
            WHERE pm.strategy_id = ?
        )";
        std::vector<BindValue> binds;
        binds.push_back(bind_int(strategy_id));

        // Date range
        if (params.from_time > 0)
        {
            sql += " AND pm.date >= date(?, 'unixepoch')";
            binds.push_back(bind_int(params.from_time));
        }
        if (params.to_time > 0)
        {
            sql += " AND pm.date <= date(?, 'unixepoch')";
            binds.push_back(bind_int(params.to_time));
        }

        sql += build_order_by_clause("pm.date", "DESC");
        sql += " LIMIT ? OFFSET ?";
        binds.push_back(bind_int(params.limit));
        binds.push_back(bind_int(params.offset));

        auto results = db_->query(sql, binds, "get_strategy_performance");

        // Calculate aggregates
        if (!results.empty())
        {
            double total_pnl = 0;
            int trade_count = 0;
            for (const auto &row : results)
            {
                total_pnl += std::stod(row.at("total_pnl"));
                trade_count += std::stoi(row.at("trade_count"));
            }

            crow::json::wvalue response = rows_to_json(results);
            response["summary"]["total_pnl"] = total_pnl;
            response["summary"]["total_trades"] = trade_count;
            response["summary"]["strategy_id"] = strategy_id;

            return crow::response(response);
        }

        return crow::response(rows_to_json(results));
    }
    catch (const std::exception &e)
    {
        return error_response(e.what());
    }
}