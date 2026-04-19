#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <hiredis/hiredis.h>
#include <spdlog/spdlog.h>
#include "Config.h"
#include "SQLiteDB.hpp"

class RedisSubscriber
{
public:
    RedisSubscriber(const RedisConfig &cfg, SQLiteDB &db)
        : m_cfg(cfg), m_db(db), m_ctx(nullptr) {}

    ~RedisSubscriber()
    {
        if (m_ctx)
            redisFree(m_ctx);
    }

    // Not copyable or movable — owns a raw redisContext* and holds a DB reference
    RedisSubscriber(const RedisSubscriber &) = delete;
    RedisSubscriber &operator=(const RedisSubscriber &) = delete;
    RedisSubscriber(RedisSubscriber &&) = delete;
    RedisSubscriber &operator=(RedisSubscriber &&) = delete;

    // Connect to Redis and subscribe to all configured channels
    bool connect()
    {
        m_ctx = redisConnect(m_cfg.host.c_str(), m_cfg.port);
        if (!m_ctx || m_ctx->err)
        {
            std::string errMsg = m_ctx ? m_ctx->errstr : "Cannot allocate Redis context";
            spdlog::error("[Redis] Connection failed: {}", errMsg);
            if (m_ctx)
            {
                redisFree(m_ctx);
                m_ctx = nullptr;
            }
            return false;
        }
        spdlog::info("[Redis] Connected to {}:{}", m_cfg.host, m_cfg.port);

        for (const auto &ch : m_cfg.channels)
        {
            redisReply *reply = static_cast<redisReply *>(
                redisCommand(m_ctx, "SUBSCRIBE %s", ch.c_str()));
            if (!reply)
            {
                spdlog::error("[Redis] Failed to subscribe to '{}'", ch);
                return false;
            }
            freeReplyObject(reply);
            spdlog::info("[Redis] Subscribed to '{}'", ch);
        }
        return true;
    }

    // Blocking event loop — runs until a fatal error
    void run()
    {
        spdlog::info("[Redis] Listening for messages...");
        while (true)
        {
            redisReply *reply = nullptr;
            if (redisGetReply(m_ctx, reinterpret_cast<void **>(&reply)) != REDIS_OK)
            {
                spdlog::error("[Redis] redisGetReply error — exiting loop");
                break;
            }
            if (reply)
            {
                processReply(reply);
            }
        }
    }

    // Process Redis reply and store in database
    void processReply(redisReply *reply);

    // Initialize database tables if they don't exist
    bool initializeDatabase();

private:
    // Database operations for different message types
    void storeRawMessage(const std::string &channel, const std::string &message);
    void storeTrade(const std::string &channel, const std::string &message);
    void storeOrderBook(const std::string &channel, const std::string &message);
    void storeTicker(const std::string &channel, const std::string &message);

    // Message parsers
    bool parseTradeMessage(const std::string &message, std::vector<BindValue> &params);
    bool parseOrderBookMessage(const std::string &message, std::vector<BindValue> &params);
    bool parseTickerMessage(const std::string &message, std::vector<BindValue> &params);

    // Helper methods
    int64_t getCurrentTimestamp();
    std::vector<std::string> splitString(const std::string &str, char delimiter);

    RedisConfig m_cfg;
    SQLiteDB &m_db; // Database reference only
    redisContext *m_ctx;
};