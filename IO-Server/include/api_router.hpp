#ifndef API_ROUTER_HPP
#define API_ROUTER_HPP

#include <string>
#include <memory>
#include <crow.h>
#include "SQLiteDB.hpp"
#include "calculation_registry.hpp"

class APIRouter
{
public:
    // calc_registry may be nullptr; if provided, /api/calc routes are enabled.
    explicit APIRouter(std::shared_ptr<SQLiteDB> db,
                       std::shared_ptr<CalculationRegistry> calc_registry = nullptr);

    // Initialize routes
    void setup_routes(crow::SimpleApp &app);

private:
    std::shared_ptr<SQLiteDB> db_;
    std::shared_ptr<CalculationRegistry> calc_registry_;

    // Helper to parse query parameters
    struct QueryParams
    {
        std::string symbol;
        std::string timeframe;
        int64_t from_time = 0;
        int64_t to_time = 0;
        int limit = 100;
        int offset = 0;
        std::string strategy;
        std::string status;
        std::string direction;
        std::string order_by;
        std::string order_dir = "ASC";
    };

    QueryParams parse_params(const crow::request &req);

    // API Handlers
    crow::response get_symbols(const crow::request &req);
    crow::response get_symbol(const crow::request &req, const std::string &ticker);

    crow::response get_ohlcv(const crow::request &req);
    crow::response get_latest_ohlcv(const crow::request &req);

    crow::response get_orderbook(const crow::request &req);
    crow::response get_orderbook_snapshot(const crow::request &req, int64_t snapshot_time);

    crow::response get_strategies(const crow::request &req);
    crow::response get_strategy(const crow::request &req, int strategy_id);
    crow::response create_strategy(const crow::request &req);
    crow::response update_strategy(const crow::request &req, int strategy_id);

    crow::response get_orders(const crow::request &req);
    crow::response get_order(const crow::request &req, int order_id);
    crow::response get_orders_by_client_id(const crow::request &req, const std::string &client_id);
    crow::response create_order(const crow::request &req);
    crow::response cancel_order(const crow::request &req, int order_id);

    crow::response get_signals(const crow::request &req);
    crow::response get_signal(const crow::request &req, int signal_id);
    crow::response create_signal(const crow::request &req);

    crow::response get_positions(const crow::request &req);
    crow::response get_position(const crow::request &req, int position_id);
    crow::response get_positions_by_strategy(const crow::request &req, int strategy_id);

    crow::response get_performance(const crow::request &req);
    crow::response get_strategy_performance(const crow::request &req, int strategy_id);

    // Helper methods
    std::string build_date_range_filter(const std::string &field, int64_t from, int64_t to);
    std::string build_order_by_clause(const std::string &default_field,
                                      const std::string &direction);
    crow::json::wvalue row_to_json(const Row &row);
    crow::json::wvalue rows_to_json(const RowSet &rows);

    // Error responses
    crow::response error_response(const std::string &message, int code = 400);
    crow::response not_found_response(const std::string &resource);

    // ── Calculation endpoints ─────────────────────────────────────────────────
    // GET /api/calc           — list all registered methods
    // GET /api/calc/<method>  — run a named calculation
    CalcParams parse_calc_params(const crow::request &req);
    crow::response list_calculations(const crow::request &req);
    crow::response run_calculation(const crow::request &req,
                                   const std::string &method);
};

#endif // API_ROUTER_HPP