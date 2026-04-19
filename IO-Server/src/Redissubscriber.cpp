#include "RedisSubscriber.hpp"
#include <sstream>
#include <chrono>
#include <iomanip>

// Helper function to get current timestamp in milliseconds
int64_t RedisSubscriber::getCurrentTimestamp()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

// Helper function to split string
std::vector<std::string> RedisSubscriber::splitString(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

// Parse trade message - expecting format: "symbol,price,quantity,timestamp,trade_id,side"
bool RedisSubscriber::parseTradeMessage(const std::string &message, std::vector<BindValue> &params)
{
    auto parts = splitString(message, ',');
    if (parts.size() < 6)
    {
        spdlog::warn("[DB] Invalid trade message format: {}", message);
        return false;
    }

    try
    {
        // Format: symbol,price,quantity,timestamp,trade_id,side
        params.push_back(bind_text(parts[0]));            // symbol
        params.push_back(bind_real(std::stod(parts[1]))); // price
        params.push_back(bind_real(std::stod(parts[2]))); // quantity
        params.push_back(bind_int(std::stoll(parts[3]))); // timestamp
        params.push_back(bind_text(parts[4]));            // trade_id
        params.push_back(bind_text(parts[5]));            // side
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("[DB] Failed to parse trade message: {} - {}", message, e.what());
        return false;
    }
}

bool RedisSubscriber::parseOrderBookMessage(const std::string &message, std::vector<BindValue> &params)
{
    auto parts = splitString(message, ',');
    if (parts.size() < 7)
    {
        spdlog::warn("[DB] Invalid orderbook message format: {}", message);
        return false;
    }

    try
    {
        // Format: symbol,side,price,quantity,timestamp,update_type,sequence
        params.push_back(bind_text(parts[0]));            // symbol
        params.push_back(bind_text(parts[1]));            // side (bid/ask)
        params.push_back(bind_real(std::stod(parts[2]))); // price
        params.push_back(bind_real(std::stod(parts[3]))); // quantity
        params.push_back(bind_int(std::stoll(parts[4]))); // timestamp
        params.push_back(bind_text(parts[5]));            // update_type
        params.push_back(bind_int(std::stoll(parts[6]))); // sequence
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("[DB] Failed to parse orderbook message: {} - {}", message, e.what());
        return false;
    }
}

// Parse ticker message - expecting format: "symbol,bid,ask,last,volume,timestamp"
bool RedisSubscriber::parseTickerMessage(const std::string &message, std::vector<BindValue> &params)
{
    auto parts = splitString(message, ',');
    if (parts.size() < 6)
    {
        spdlog::warn("[DB] Invalid ticker message format: {}", message);
        return false;
    }

    try
    {
        // Format: symbol,bid,ask,last,volume,timestamp
        params.push_back(bind_text(parts[0]));            // symbol
        params.push_back(bind_real(std::stod(parts[1]))); // bid
        params.push_back(bind_real(std::stod(parts[2]))); // ask
        params.push_back(bind_real(std::stod(parts[3]))); // last
        params.push_back(bind_real(std::stod(parts[4]))); // volume
        params.push_back(bind_int(std::stoll(parts[5]))); // timestamp
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("[DB] Failed to parse ticker message: {} - {}", message, e.what());
        return false;
    }
}

// Store raw message for auditing
void RedisSubscriber::storeRawMessage(const std::string &channel, const std::string &message)
{
    try
    {
        std::string sql = R"(
            INSERT INTO raw_messages (channel, message, received_at, processed)
            VALUES (?, ?, ?, 0)
        )";

        std::vector<BindValue> params = {
            bind_text(channel),
            bind_text(message),
            bind_int(getCurrentTimestamp())};

        m_db.exec(sql, params, "store_raw_message");
        spdlog::debug("[DB] Stored raw message from channel: {}", channel);
    }
    catch (const std::exception &e)
    {
        spdlog::error("[DB] Failed to store raw message: {}", e.what());
    }
}

// Store trade data
void RedisSubscriber::storeTrade(const std::string &channel, const std::string &message)
{
    std::vector<BindValue> params;

    if (!parseTradeMessage(message, params))
    {
        storeRawMessage(channel, message); // Store raw for debugging
        return;
    }

    try
    {
        std::string sql = R"(
            INSERT INTO trades (symbol, price, quantity, timestamp, trade_id, side, created_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        )";

        // Add created_at timestamp
        params.push_back(bind_int(getCurrentTimestamp()));

        m_db.exec(sql, params, "store_trade");

        spdlog::info("[DB] Stored trade: {} {} @ {} (qty: {})",
                     params[0].sval,  // symbol
                     params[5].sval,  // side
                     params[1].dval,  // price
                     params[2].dval); // quantity

        // Also update the raw message as processed
        std::string update_sql = R"(
            UPDATE raw_messages SET processed = 1 
            WHERE message = ? AND channel = ? AND processed = 0
        )";

        std::vector<BindValue> update_params = {
            bind_text(message),
            bind_text(channel)};

        m_db.exec(update_sql, update_params, "update_raw_message");
    }
    catch (const std::exception &e)
    {
        spdlog::error("[DB] Failed to store trade: {}", e.what());
        storeRawMessage(channel, message); // Store raw on error
    }
}

// Store orderbook data
void RedisSubscriber::storeOrderBook(const std::string &channel, const std::string &message)
{
    std::vector<BindValue> params;

    if (!parseOrderBookMessage(message, params))
    {
        storeRawMessage(channel, message);
        return;
    }

    try
    {
        std::string sql = R"(
            INSERT INTO orderbook_updates (symbol, side, price, quantity, timestamp, update_type, sequence, created_at)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        )";

        // Add created_at timestamp
        params.push_back(bind_int(getCurrentTimestamp()));

        m_db.exec(sql, params, "store_orderbook");

        spdlog::debug("[DB] Stored orderbook update for {}: {} @ {} (seq: {})",
                      params[0].sval,  // symbol
                      params[1].sval,  // side
                      params[2].dval,  // price
                      params[6].ival); // sequence
    }
    catch (const std::exception &e)
    {
        spdlog::error("[DB] Failed to store orderbook: {}", e.what());
        storeRawMessage(channel, message);
    }
}

// Store ticker data
void RedisSubscriber::storeTicker(const std::string &channel, const std::string &message)
{
    std::vector<BindValue> params;

    if (!parseTickerMessage(message, params))
    {
        storeRawMessage(channel, message);
        return;
    }

    try
    {
        std::string sql = R"(
            INSERT INTO ticker_updates (symbol, bid, ask, last, volume, timestamp, created_at)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        )";

        // Add created_at timestamp
        params.push_back(bind_int(getCurrentTimestamp()));

        m_db.exec(sql, params, "store_ticker");

        spdlog::debug("[DB] Stored ticker for {}: bid={}, ask={}, last={}",
                      params[0].sval,  // symbol
                      params[1].dval,  // bid
                      params[2].dval,  // ask
                      params[3].dval); // last
    }
    catch (const std::exception &e)
    {
        spdlog::error("[DB] Failed to store ticker: {}", e.what());
        storeRawMessage(channel, message);
    }
}

void RedisSubscriber::processReply(redisReply *reply)
{
    if (!reply || reply->type != REDIS_REPLY_ARRAY || reply->elements < 3)
    {
        spdlog::warn("[Redis] Invalid reply format");
        if (reply)
            freeReplyObject(reply);
        return;
    }

    // Standard Redis Pub/Sub message format:
    // [ "message", channel, message_string ]
    if (reply->elements >= 3 &&
        reply->element[0] && reply->element[0]->str &&
        std::string(reply->element[0]->str) == "message")
    {

        std::string channel = reply->element[1] ? reply->element[1]->str : "";
        std::string message = reply->element[2] ? reply->element[2]->str : "";

        spdlog::debug("[Redis] Received message on channel '{}': {}", channel, message);

        // Store raw message first (for auditing)
        storeRawMessage(channel, message);

        // Route message to the appropriate table based on channel name
        if (channel.find("trade") != std::string::npos || channel == "trades")
        {
            storeTrade(channel, message);
        }
        else if (channel.find("orderbook") != std::string::npos || channel == "orderbook")
        {
            storeOrderBook(channel, message);
        }
        else if (channel.find("ticker") != std::string::npos || channel == "tickers")
        {
            storeTicker(channel, message);
        }
        else
        {
            spdlog::debug("[DB] No specific handler for channel '{}', stored as raw", channel);
        }

        freeReplyObject(reply);
        return;
    }

    // Handle other Redis reply types (e.g., subscribe confirmation)
    if (reply->element[0] && reply->element[0]->str)
    {
        std::string reply_type = reply->element[0]->str;
        if (reply_type == "subscribe" && reply->elements >= 3)
        {
            std::string channel = reply->element[1] ? reply->element[1]->str : "";
            spdlog::info("[Redis] Subscribed to channel: {}", channel);
        }
    }

    freeReplyObject(reply);
}