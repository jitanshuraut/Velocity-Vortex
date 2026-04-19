# C4 Architecture Diagrams — Velocity-Vortex IO-Server

Three levels: System Context → Container → Component.  
All diagrams use the [C4-PlantUML](https://github.com/plantuml-stdlib/C4-PlantUML) style
rendered as Mermaid `C4Context` / `C4Container` / `C4Component` blocks.

---

## Level 1 — System Context

```mermaid
C4Context
    title System Context — Velocity-Vortex IO-Server

    Person(trader, "Quant / Client", "Consumes market data and analytics via REST")

    System_Boundary(vv, "Velocity-Vortex Platform") {
        System(io_server, "IO-Server", "Ingests MQ data, persists to SQLite, serves REST API (C++20 / Crow)")
        System(algo_engine, "AlgoEngine", "Generates trading signals")
        System(order_mgr, "OrderManager", "Manages order lifecycle")
        System(risk, "RiskAnalysis", "Real-time risk checks")
        System(backtester, "Backtesting-Bot", "Historical strategy simulation")
    }

    System_Ext(exchange, "Crypto / Equity Exchange", "Publishes live market data")
    System_Ext(redis, "Redis", "Message queue broker (pub/sub)")

    Rel(exchange, redis, "Publishes trades, orderbook, ticker updates", "Redis pub/sub")
    Rel(redis, io_server, "Delivers market data messages", "hiredis / SUBSCRIBE")
    Rel(io_server, algo_engine, "OHLCV + analytics REST API", "HTTP/JSON")
    Rel(io_server, order_mgr, "Order history + positions REST API", "HTTP/JSON")
    Rel(io_server, risk, "Position + performance data", "HTTP/JSON")
    Rel(io_server, backtester, "Historical OHLCV data", "HTTP/JSON")
    Rel(trader, io_server, "Query market data & analytics", "HTTP/JSON :8080")
```

---

## Level 2 — Container

```mermaid
C4Container
    title Container Diagram — IO-Server

    System_Ext(redis, "Redis 7", "Pub/sub message broker")
    Person(client, "API Consumer", "AlgoEngine / Risk / Trader")

    System_Boundary(io, "IO-Server Process  (IOBroker.exe)") {

        Container(http_server, "HTTP Server", "Crow C++ Framework",
            "Listens on :8080, routes all REST requests")

        Container(api_router, "APIRouter", "C++ class",
            "Implements every /api/* route handler. Dispatches calc requests to CalculationRegistry")

        Container(calc_registry, "CalculationRegistry", "C++ class",
            "Plugin registry: maps method name → ICalculationHandler. Extensible without changing router")

        Container(rca, "RCAHandler", "ICalculationHandler",
            "Rate of Change Analysis — momentum over rolling window")

        Container(mca, "MCAHandler", "ICalculationHandler",
            "Moving Correlation Analysis — Pearson correlation between two symbols")

        Container(redis_sub, "RedisSubscriber", "C++ class / detached thread",
            "Subscribes to Redis channels, parses CSV messages, writes to SQLite. Optional — server runs without it")

        ContainerDb(sqlite, "SQLite Database", "velocity_vortex.db",
            "All time-series and relational data. 12 tables. WAL mode, 64 MB cache")
    }

    Rel(redis, redis_sub, "Push messages", "hiredis blocking redisGetReply()")
    Rel(redis_sub, sqlite, "INSERT trades / orderbook_updates / ticker_updates / raw_messages", "SQLiteDB::exec()")

    Rel(client, http_server, "GET /api/*", "HTTP :8080")
    Rel(http_server, api_router, "Dispatch request")
    Rel(api_router, sqlite, "SELECT / INSERT / UPDATE", "SQLiteDB::query() / exec()")
    Rel(api_router, calc_registry, "Lookup + invoke handler", "/api/calc/<method>")
    Rel(calc_registry, rca, "compute(CalcParams, SQLiteDB)")
    Rel(calc_registry, mca, "compute(CalcParams, SQLiteDB)")
    Rel(rca, sqlite, "SELECT ohlcv", "SQLiteDB::query()")
    Rel(mca, sqlite, "SELECT ohlcv × 2 symbols", "SQLiteDB::query()")
```

---

## Level 3 — Component (APIRouter)

```mermaid
C4Component
    title Component Diagram — APIRouter

    Container_Ext(crow, "Crow HTTP Server", "Registers CROW_ROUTE macros")
    Container_Ext(db, "SQLiteDB", "PIMPL wrapper around sqlite3*")
    Container_Ext(registry, "CalculationRegistry", "Name → handler map")

    Container_Boundary(router, "APIRouter  (api_router.cpp)") {

        Component(setup, "setup_routes()", "Registers all CROW_ROUTE lambdas at startup")

        Component(sym, "Symbol Handlers", "get_symbols(), get_symbol(ticker)")
        Component(ohlcv, "OHLCV Handlers", "get_ohlcv(), get_latest_ohlcv()")
        Component(ob, "Orderbook Handlers", "get_orderbook(), get_orderbook_snapshot(time)")
        Component(strat, "Strategy Handlers", "get/create/update_strategy()")
        Component(ord, "Order Handlers", "get/create/cancel_order(), get_orders_by_client_id()")
        Component(sig, "Signal Handlers", "get/create_signal()")
        Component(pos, "Position Handlers", "get_positions(), get_positions_by_strategy()")
        Component(perf, "Performance Handlers", "get_performance(), get_strategy_performance()")

        Component(calc, "Calc Handlers", "list_calculations(), run_calculation(method)")
        Component(params, "parse_params()", "Extracts symbol/timeframe/from/to/limit/offset from query string")
        Component(calc_params, "parse_calc_params()", "Extracts symbol/symbol2/window + forwards extras to CalcParams::extra")
        Component(helpers, "Response Helpers", "rows_to_json(), row_to_json(), error_response(), not_found_response()")
    }

    Rel(crow, setup, "Calls at startup")
    Rel(setup, sym, "Registers /api/symbols routes")
    Rel(setup, ohlcv, "Registers /api/ohlcv routes")
    Rel(setup, ob, "Registers /api/orderbook routes")
    Rel(setup, strat, "Registers /api/strategies routes")
    Rel(setup, ord, "Registers /api/orders routes")
    Rel(setup, sig, "Registers /api/signals routes")
    Rel(setup, pos, "Registers /api/positions routes")
    Rel(setup, perf, "Registers /api/performance routes")
    Rel(setup, calc, "Registers /api/calc routes (if registry ≠ nullptr)")

    Rel(sym, db, "SELECT symbols")
    Rel(ohlcv, db, "SELECT ohlcv JOIN symbols")
    Rel(ob, db, "SELECT orderbook JOIN symbols")
    Rel(strat, db, "SELECT/INSERT/UPDATE strategy")
    Rel(ord, db, "SELECT/INSERT/UPDATE orders")
    Rel(sig, db, "SELECT/INSERT signal")
    Rel(pos, db, "SELECT positions")
    Rel(perf, db, "SELECT performance_metrics")
    Rel(calc, registry, "get(method) → ICalculationHandler::compute()")
    Rel(calc, calc_params, "parse_calc_params(req)")
    Rel(sym, helpers, "rows_to_json / error_response")
    Rel(ohlcv, helpers, "rows_to_json / error_response")
```

---

## Level 3 — Component (RedisSubscriber)

```mermaid
C4Component
    title Component Diagram — RedisSubscriber

    Container_Ext(redis_broker, "Redis Broker", "External pub/sub server")
    Container_Ext(db2, "SQLiteDB", "PIMPL sqlite3 wrapper")

    Container_Boundary(sub, "RedisSubscriber  (Redissubscriber.cpp)") {

        Component(connect, "connect()", "redisConnect() + SUBSCRIBE to each channel from RedisConfig")
        Component(run_loop, "run()", "Blocking loop: redisGetReply() → processReply(). Thread-safe via detached thread in main.cpp")
        Component(proc, "processReply()", "Routes array reply [message, channel, payload] to correct store* method")

        Component(parse_trade, "parseTradeMessage()", "CSV: symbol,price,qty,timestamp,trade_id,side → BindValue vector")
        Component(parse_ob, "parseOrderBookMessage()", "CSV: symbol,side,price,qty,timestamp,update_type,sequence → BindValue vector")
        Component(parse_tick, "parseTickerMessage()", "CSV: symbol,bid,ask,last,volume,timestamp → BindValue vector")

        Component(store_trade, "storeTrade()", "INSERT INTO trades")
        Component(store_ob, "storeOrderBook()", "INSERT INTO orderbook_updates")
        Component(store_tick, "storeTicker()", "INSERT INTO ticker_updates")
        Component(store_raw, "storeRawMessage()", "INSERT INTO raw_messages — audit log, always called first")
    }

    Rel(redis_broker, connect, "TCP connection + SUBSCRIBE ACK")
    Rel(connect, run_loop, "Starts blocking loop after successful connect")
    Rel(run_loop, proc, "Each reply")
    Rel(proc, store_raw, "Always — audit every message")
    Rel(proc, store_trade, "channel contains 'trade'")
    Rel(proc, store_ob, "channel contains 'orderbook'")
    Rel(proc, store_tick, "channel contains 'ticker'")
    Rel(store_trade, parse_trade, "parse CSV first")
    Rel(store_ob, parse_ob, "parse CSV first")
    Rel(store_tick, parse_tick, "parse CSV first")
    Rel(store_trade, db2, "SQLiteDB::exec() INSERT trades")
    Rel(store_ob, db2, "SQLiteDB::exec() INSERT orderbook_updates")
    Rel(store_tick, db2, "SQLiteDB::exec() INSERT ticker_updates")
    Rel(store_raw, db2, "SQLiteDB::exec() INSERT raw_messages")
```

---

## Level 3 — Component (Calculation Framework)

```mermaid
C4Component
    title Component Diagram — Calculation Framework

    Container_Ext(router2, "APIRouter", "Calls list_calculations() and run_calculation()")
    Container_Ext(db3, "SQLiteDB", "Data source for all calculations")

    Container_Boundary(fw, "Calculation Framework") {

        Component(iface, "ICalculationHandler", "Abstract interface\ncompute(CalcParams, SQLiteDB) → crow::json::wvalue\nname() → string\ndescription() → string")

        Component(reg, "CalculationRegistry", "std::unordered_map<string, shared_ptr<ICalculationHandler>>\nregister_handler() / get(name) / list() / list_json()")

        Component(rca2, "RCAHandler", "name=rca\nROC(i) = (close[i] − close[i−n]) / close[i−n] × 100\nParams: symbol, timeframe, window(def=14), limit, from, to")

        Component(mca2, "MCAHandler", "name=mca\nRolling Pearson correlation between symbol & symbol2\nParams: symbol, symbol2, timeframe, window(def=20), limit, from, to")

        Component(future, "YourHandler (example)", "Implement ICalculationHandler\nRegister in main.cpp\nNo other file changes needed")
    }

    Rel(router2, reg, "GET /api/calc → list_json()")
    Rel(router2, reg, "GET /api/calc/<method> → get(method)")
    Rel(reg, rca2, "compute(params, db)")
    Rel(reg, mca2, "compute(params, db)")
    Rel(reg, future, "compute(params, db)")
    Rel(rca2, db3, "SELECT ohlcv WHERE ticker=symbol ORDER BY open_time ASC")
    Rel(mca2, db3, "SELECT ohlcv for symbol AND symbol2, align on open_time")
    Rel(rca2, iface, "implements")
    Rel(mca2, iface, "implements")
    Rel(future, iface, "implements")
```
