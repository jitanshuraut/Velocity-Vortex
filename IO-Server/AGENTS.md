# IO-Server — AI Agent Context

## What This Project Is

IO-Server is the **IO layer of the Velocity-Vortex HFT platform**. Its sole responsibility is:

1. **Ingest** trade/orderbook/ticker data from a Redis message queue
2. **Persist** all time-series data to a local SQLite database
3. **Serve** that data (plus pre-computed analytics) over a REST HTTP API

It owns ALL storage and ALL IO. Other components in the wider Velocity-Vortex monorepo (AlgoEngine, OrderManager, RiskAnalysis, etc.) are consumers of this server.

---

## Project Layout

```
IO-Server/
├── CMakeLists.txt          # Build: C++20, MSVC/Ninja, vcpkg
├── CMakeSettings.json      # VS IDE config (x64-Debug → out/build/x64-Debug)
├── .vs/launch.vs.json      # VS debugger target: IOBroker.exe
│
├── include/                # All headers (added to target_include_directories)
│   ├── Config.h            # RedisConfig, ServerConfig structs
│   ├── SQLiteDB.hpp        # PIMPL SQLite wrapper + BindValue type
│   ├── Redissubscriber.hpp # Redis subscriber class declaration
│   ├── api_router.hpp      # Crow HTTP router class
│   ├── calculation_handler.hpp     # ICalculationHandler interface + CalcParams
│   ├── calculation_registry.hpp    # CalculationRegistry (name → handler map)
│   └── calculations/
│       ├── rca_handler.hpp         # Rate of Change Analysis
│       └── mca_handler.hpp         # Moving Correlation Analysis
│
├── src/                    # All implementations
│   ├── main.cpp            # Entry point: wires DB + Redis thread + HTTP server
│   ├── SQLiteDB.cpp        # SQLite PIMPL impl (exec/query/bootstrap)
│   ├── Redissubscriber.cpp # Redis subscriber: parse & store messages
│   ├── api_router.cpp      # All HTTP route handlers + calc dispatch
│   ├── calculation_registry.cpp
│   ├── subscriber.cpp      # OLD prototype (has its own main, NOT compiled into IOBroker)
│   └── calculations/
│       ├── rca_handler.cpp
│       └── mca_handler.cpp
│
└── Schema/
    └── bootstrap.sql       # SQLite schema (pure SQLite syntax — NOT SQL Server)
```

---

## Build System

- **Toolchain**: MSVC, CMake 3.15+, vcpkg (`C:/Users/lenovo/vcpkg`)
- **Triplet**: `x64-windows`
- **Standard**: C++20
- **Output exe**: `out/build/x64-Debug/bin/IOBroker.exe`
- **POST_BUILD**: `Schema/` is automatically copied next to the exe
- **vcpkg packages**: `hiredis`, `unofficial-sqlite3`, `spdlog`, `fmt`, `Crow`, `nlohmann_json`

> `subscriber.cpp` has its own `main()` and is intentionally excluded from `ALL_SOURCES`. It is a legacy
> prototype and is compiled into a separate `subscriber.exe` by an older CMake config. Do not add it to
> `ALL_SOURCES`.

---

## Runtime Configuration

All config lives in **`include/Config.h`** as plain structs (no config file yet):

```cpp
ServerConfig {
    db_path    = "./velocity_vortex.db"   // relative to exe, i.e. out/.../bin/
    schema_dir = "./Schema"               // relative to exe — copied by POST_BUILD
    http_port  = 8080
    enable_cors = true
}

RedisConfig {
    host     = "127.0.0.1"
    port     = 6379
    channels = { "trades", "orderbook", "tickers" }
}
```

---

## Data Flow

```
Redis channels          RedisSubscriber           SQLite (velocity_vortex.db)
─────────────           ───────────────           ───────────────────────────
"trades"      ──parse─► storeTrade()     ──INSERT► trades
"orderbook"   ──parse─► storeOrderBook() ──INSERT► orderbook_updates
"tickers"     ──parse─► storeTicker()    ──INSERT► ticker_updates
any channel   ──audit─► storeRawMessage()──INSERT► raw_messages

                              ▲ all failures are logged and swallowed
                              ▲ Redis is OPTIONAL — HTTP server runs regardless
```

---

## REST API (Crow, port 8080)

| Method  | Path                               | Description                                         |
| ------- | ---------------------------------- | --------------------------------------------------- |
| GET     | `/api/symbols`                     | List symbols (filter: `?symbol=`)                   |
| GET     | `/api/symbols/<ticker>`            | Single symbol                                       |
| GET     | `/api/ohlcv`                       | OHLCV bars (`?symbol=&timeframe=&from=&to=&limit=`) |
| GET     | `/api/ohlcv/latest`                | Most recent bar                                     |
| GET     | `/api/orderbook`                   | Orderbook snapshots                                 |
| GET     | `/api/orderbook/snapshot/<time>`   | Single snapshot at unix-ms                          |
| GET     | `/api/strategies`                  | CRUD for strategies                                 |
| POST    | `/api/strategies`                  | Create strategy                                     |
| GET/PUT | `/api/strategies/<id>`             | Get/update strategy                                 |
| GET     | `/api/orders`                      | Order list                                          |
| POST    | `/api/orders`                      | Create order                                        |
| GET     | `/api/orders/<id>`                 | Single order                                        |
| GET     | `/api/orders/client/<id>`          | Orders by client ID                                 |
| POST    | `/api/orders/<id>/cancel`          | Cancel order                                        |
| GET     | `/api/signals`                     | Signals                                             |
| POST    | `/api/signals`                     | Create signal                                       |
| GET     | `/api/positions`                   | Positions                                           |
| GET     | `/api/strategies/<id>/positions`   | Positions for strategy                              |
| GET     | `/api/performance`                 | Performance metrics                                 |
| GET     | `/api/strategies/<id>/performance` | Strategy performance                                |
| GET     | `/api/calc`                        | **List all registered calculation methods**         |
| GET     | `/api/calc/<method>`               | **Run a named calculation**                         |

### Common query parameters

`symbol`, `symbol2`, `timeframe`, `from` (unix-ms), `to` (unix-ms), `limit`, `offset`, `window`, `order_by`, `order_dir`

---

## Calculation Framework (open/closed principle)

New analytics methods can be added **without touching any existing file** except `main.cpp`:

1. Create `include/calculations/my_handler.hpp` inheriting `ICalculationHandler`
2. Create `src/calculations/my_handler.cpp` implementing `compute()`, `name()`, `description()`
3. Add to `CMakeLists.txt` `ALL_SOURCES`
4. In `main.cpp`: `registry->register_handler(std::make_shared<MyHandler>());`

**Built-in methods:**

| Name  | Endpoint                                                  | Description                                                  |
| ----- | --------------------------------------------------------- | ------------------------------------------------------------ |
| `rca` | `/api/calc/rca?symbol=X&timeframe=1m&window=14`           | Rate of Change: `(close[i] - close[i-n]) / close[i-n] * 100` |
| `mca` | `/api/calc/mca?symbol=X&symbol2=Y&timeframe=1h&window=20` | Rolling Pearson correlation between two symbols              |

---

## Database Schema (SQLite)

**Important**: Schema is SQLite syntax only. No T-SQL, no `dbo.`, no `IDENTITY`, no `DATETIME2`.

Core tables (seeded from `Schema/bootstrap.sql` at startup):

| Table                 | Purpose                                                 |
| --------------------- | ------------------------------------------------------- |
| `symbols`             | Master instrument list                                  |
| `ohlcv`               | OHLCV bars keyed by `(symbol_id, timeframe, open_time)` |
| `orderbook`           | Level-2 snapshot entries                                |
| `orderbook_updates`   | Real-time tick updates from MQ                          |
| `strategy`            | Strategy definitions                                    |
| `orders`              | All orders (live + historical)                          |
| `signal`              | Trading signals from strategies                         |
| `positions`           | Open positions                                          |
| `performance_metrics` | Daily strategy performance                              |
| `trades`              | Individual tick trades from MQ                          |
| `ticker_updates`      | Best-bid/ask snapshots from MQ                          |
| `raw_messages`        | Audit log of every inbound MQ message                   |

Timestamps for MQ tables (`trades`, `ticker_updates`, `orderbook_updates`, `raw_messages`) are **unix milliseconds as INTEGER**.  
Timestamps for relational tables (`symbols`, `strategy`, `orders`, etc.) are **ISO-8601 TEXT** via `strftime()`.

---

## Key Design Decisions & Constraints

- **Redis is optional**: if `RedisSubscriber::connect()` fails, the thread exits with a `warn` log and the HTTP server continues normally.
- **Bootstrap is idempotent**: all tables use `CREATE TABLE IF NOT EXISTS`; safe to run on every startup.
- **Schema must be co-located with exe**: copied by CMake POST_BUILD. Do NOT use `PROJECT_SOURCE_DIR`-relative paths at runtime.
- **`execute_script` uses `sqlite3_exec()`**: the old semicolon-splitter was replaced because it broke `CREATE TRIGGER … BEGIN … END` blocks.
- **PIMPL on SQLiteDB**: the `sqlite3*` handle is inside `SQLiteDB::Impl`. Use `pimpl->db`, not a bare `db_` member.
- **`subscriber.cpp`** is legacy dead code that is NOT part of the `IOBroker` target.
- **`APIRouter` takes a `shared_ptr<CalculationRegistry>`**: if `nullptr` is passed, `/api/calc` routes are simply not registered.
- **Response shape**: success → `{ success: true, data: [...], count: N }`, error → `{ success: false, error: "...", code: N }`.
