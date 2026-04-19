#include <thread>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Config.h"
#include "SQLiteDB.hpp"
#include "Redissubscriber.hpp"
#include "api_router.hpp"
#include "calculation_registry.hpp"
#include "calculations/rca_handler.hpp"
#include "calculations/mca_handler.hpp"

// ─── Build and populate the calculation registry ─────────────────────────────
//
// To add a new calculation method:
//   1. Implement ICalculationHandler in include/calculations/ + src/calculations/
//   2. #include its header here
//   3. registry->register_handler(std::make_shared<YourHandler>());
//
static std::shared_ptr<CalculationRegistry> build_registry()
{
    auto registry = std::make_shared<CalculationRegistry>();
    registry->register_handler(std::make_shared<RCAHandler>());
    registry->register_handler(std::make_shared<MCAHandler>());
    return registry;
}

int main()
{
    // ── Logging ───────────────────────────────────────────────────────────────
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(console);
    spdlog::set_level(spdlog::level::info);
    spdlog::info("Velocity-Vortex IO-Server starting up");

    // ── Configuration ─────────────────────────────────────────────────────────
    // Defaults are defined in Config.h — no overrides needed here.
    ServerConfig srv_cfg;
    RedisConfig redis_cfg;

    // ── Database ──────────────────────────────────────────────────────────────
    auto db = std::make_shared<SQLiteDB>(srv_cfg.db_path);
    spdlog::info("Database opened: {}", srv_cfg.db_path);

    try
    {
        db->bootstrap_from_directory(srv_cfg.schema_dir);
        spdlog::info("Schema bootstrapped from: {}", srv_cfg.schema_dir);
    }
    catch (const std::exception &e)
    {
        spdlog::warn("Schema bootstrap skipped or partially applied: {}", e.what());
    }

    // ── Calculation registry ──────────────────────────────────────────────────
    auto calc_registry = build_registry();
    spdlog::info("Registered calculation methods: {}",
                 [&]()
                 {
                     std::string s;
                     for (const auto &n : calc_registry->list())
                         s += n + " ";
                     return s;
                 }());

    // ── Redis subscriber (optional — HTTP server runs regardless) ────────────
    // If Redis is unavailable the subscriber thread exits cleanly and the
    // REST/calculation APIs continue to serve requests from the local DB.
    auto redis_sub = std::make_shared<RedisSubscriber>(redis_cfg, *db);

    std::thread redis_thread([&redis_sub]()
                             {
        try {
            if (!redis_sub->connect()) {
                spdlog::warn("[Redis] Subscriber unavailable — continuing without message queue");
                return;
            }
            redis_sub->run();
        } catch (const std::exception& e) {
            spdlog::error("[Redis] Subscriber thread threw: {}", e.what());
        } });
    redis_thread.detach();

    // ── HTTP server ───────────────────────────────────────────────────────────
    crow::SimpleApp app;

    if (srv_cfg.enable_cors)
    {
        app.route_dynamic("/")([](const crow::request &, crow::response &res)
                               {
            res.add_header("Access-Control-Allow-Origin", "*");
            res.end("Velocity-Vortex IO-Server"); });
    }

    APIRouter router(db, calc_registry);
    router.setup_routes(app);

    spdlog::info("HTTP server listening on port {}", srv_cfg.http_port);
    app.port(srv_cfg.http_port).multithreaded().run();

    return 0;
}
