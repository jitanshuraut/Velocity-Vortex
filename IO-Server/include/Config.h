#pragma once
#include <string>
#include <vector>

// ─── Redis Configuration ──────────────────────────────────────────────────────
struct RedisConfig
{
    std::string host = "127.0.0.1";
    int port = 6379;
    std::vector<std::string> channels = {"trades", "orderbook", "tickers"};
};

// ─── HTTP Server Configuration ────────────────────────────────────────────────
struct ServerConfig
{
    std::string db_path = "./velocity_vortex.db";
    std::string schema_dir = "./Schema";
    int http_port = 8080;
    bool enable_cors = true;
};
