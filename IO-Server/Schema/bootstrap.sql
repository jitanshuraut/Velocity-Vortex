-- =============================================================================
-- Velocity-Vortex IO-Server  â€”  SQLite schema
-- =============================================================================
-- SQLite Notes:
--   * INTEGER PRIMARY KEY AUTOINCREMENT  (not IDENTITY)
--   * TEXT  instead of NVARCHAR / VARCHAR
--   * REAL  instead of FLOAT / DOUBLE
--   * INTEGER unix-ms  instead of DATETIME2
--   * DEFAULT (strftime(...))  for text timestamps
--   * CREATE TABLE IF NOT EXISTS  (no IF/BEGIN/END blocks)
--   * No schema prefix (no dbo.)
--   * Named DEFAULT constraints not supported; use bare DEFAULT value
-- =============================================================================

PRAGMA foreign_keys = ON;

-- -----------------------------------------------------------------------------
-- Table: symbols
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS symbols (
    symbol_id      INTEGER PRIMARY KEY AUTOINCREMENT,
    ticker         TEXT    NOT NULL,
    exchange       TEXT    NOT NULL,
    asset_class    TEXT    NOT NULL DEFAULT 'EQUITY',
    base_currency  TEXT    NULL,
    quote_currency TEXT    NULL,
    lot_size       REAL    NOT NULL DEFAULT 1.0,
    tick_size      REAL    NOT NULL DEFAULT 0.01,
    contract_size  REAL    NOT NULL DEFAULT 1.0,
    min_quantity   REAL    NOT NULL DEFAULT 0.01,
    max_quantity   REAL    NULL,
    is_active      INTEGER NOT NULL DEFAULT 1,
    listed_date    TEXT    NULL,
    delisted_date  TEXT    NULL,
    created_at     TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%SZ','now')),
    updated_at     TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%SZ','now')),
    CONSTRAINT UQ_symbols_ticker UNIQUE (ticker)
);

-- -----------------------------------------------------------------------------
-- Table: ohlcv
-- open/high/low/close are SQL reserved words â€” quote with double-quotes
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS ohlcv (
    ohlcv_id     INTEGER PRIMARY KEY AUTOINCREMENT,
    symbol_id    INTEGER NOT NULL,
    timeframe    TEXT    NOT NULL,
    open_time    INTEGER NOT NULL,
    close_time   INTEGER NOT NULL,
    "open"       REAL    NOT NULL,
    "high"       REAL    NOT NULL,
    "low"        REAL    NOT NULL,
    "close"      REAL    NOT NULL,
    volume       REAL    NOT NULL DEFAULT 0.0,
    quote_volume REAL    NOT NULL DEFAULT 0.0,
    trade_count  INTEGER NOT NULL DEFAULT 0,
    vwap         REAL    NULL,
    is_closed    INTEGER NOT NULL DEFAULT 1,
    created_at   TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%SZ','now')),
    CONSTRAINT FK_ohlcv_symbol         FOREIGN KEY (symbol_id) REFERENCES symbols(symbol_id) ON DELETE CASCADE,
    CONSTRAINT UQ_ohlcv_symbol_tf_time UNIQUE (symbol_id, timeframe, open_time)
);

-- -----------------------------------------------------------------------------
-- Table: orderbook  (level-2 snapshot entries)
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS orderbook (
    ob_id         INTEGER PRIMARY KEY AUTOINCREMENT,
    symbol_id     INTEGER NOT NULL,
    snapshot_time INTEGER NOT NULL,
    side          TEXT    NOT NULL,
    price         REAL    NOT NULL,
    quantity      REAL    NOT NULL,
    level         INTEGER NOT NULL,
    num_orders    INTEGER NOT NULL DEFAULT 1,
    exchange_seq  INTEGER NULL,
    created_at    TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%SZ','now')),
    CONSTRAINT FK_orderbook_symbol FOREIGN KEY (symbol_id) REFERENCES symbols(symbol_id) ON DELETE CASCADE,
    CONSTRAINT CHK_orderbook_side  CHECK (side IN ('BID','ASK'))
);

-- -----------------------------------------------------------------------------
-- Table: orderbook_updates  (real-time tick updates from the message queue)
-- Columns match INSERT in Redissubscriber.cpp::storeOrderBook()
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS orderbook_updates (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    symbol      TEXT    NOT NULL,
    side        TEXT    NOT NULL,
    price       REAL    NOT NULL,
    quantity    REAL    NOT NULL,
    timestamp   INTEGER NOT NULL,
    update_type TEXT    NULL,
    sequence    INTEGER NULL,
    created_at  INTEGER NOT NULL DEFAULT (strftime('%s','now') * 1000)
);

-- -----------------------------------------------------------------------------
-- Table: strategy
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS strategy (
    strategy_id    INTEGER PRIMARY KEY AUTOINCREMENT,
    name           TEXT    NOT NULL,
    version        TEXT    NOT NULL DEFAULT '1.0.0',
    description    TEXT    NULL,
    author         TEXT    NULL,
    status         TEXT    NOT NULL DEFAULT 'INACTIVE',
    mode           TEXT    NOT NULL DEFAULT 'PAPER',
    capital_alloc  REAL    NOT NULL DEFAULT 0.0,
    max_position   REAL    NOT NULL DEFAULT 0.0,
    max_drawdown   REAL    NOT NULL DEFAULT 0.05,
    risk_per_trade REAL    NOT NULL DEFAULT 0.01,
    params_json    TEXT    NULL,
    started_at     INTEGER NULL,
    stopped_at     INTEGER NULL,
    created_at     TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%SZ','now')),
    updated_at     TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%SZ','now')),
    CONSTRAINT UQ_strategy_name    UNIQUE (name),
    CONSTRAINT CHK_strategy_status CHECK (status IN ('ACTIVE','INACTIVE','PAUSED','DRAFT','ARCHIVED')),
    CONSTRAINT CHK_strategy_mode   CHECK (mode   IN ('PAPER','LIVE','BACKTEST','SANDBOX'))
);

-- -----------------------------------------------------------------------------
-- Table: orders
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS orders (
    order_id          INTEGER PRIMARY KEY AUTOINCREMENT,
    client_order_id   TEXT    NOT NULL,
    exchange_order_id TEXT    NULL,
    symbol_id         INTEGER NOT NULL,
    strategy_id       INTEGER NULL,
    order_type        TEXT    NOT NULL,
    side              TEXT    NOT NULL,
    status            TEXT    NOT NULL DEFAULT 'NEW',
    price             REAL    NULL,
    stop_price        REAL    NULL,
    quantity          REAL    NOT NULL,
    filled_qty        REAL    NOT NULL DEFAULT 0.0,
    avg_fill_price    REAL    NULL,
    commission        REAL    NOT NULL DEFAULT 0.0,
    commission_asset  TEXT    NULL,
    time_in_force     TEXT    NOT NULL DEFAULT 'GTC',
    expire_time       INTEGER NULL,
    submitted_at      INTEGER NULL,
    accepted_at       INTEGER NULL,
    filled_at         INTEGER NULL,
    cancelled_at      INTEGER NULL,
    reject_reason     TEXT    NULL,
    notes             TEXT    NULL,
    created_at        TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%SZ','now')),
    updated_at        TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%SZ','now')),
    CONSTRAINT FK_orders_symbol      FOREIGN KEY (symbol_id)   REFERENCES symbols(symbol_id),
    CONSTRAINT FK_orders_strategy    FOREIGN KEY (strategy_id) REFERENCES strategy(strategy_id),
    CONSTRAINT UQ_orders_client_id   UNIQUE (client_order_id),
    CONSTRAINT CHK_orders_order_type CHECK (order_type    IN ('MARKET','LIMIT','STOP','STOP_LIMIT','TRAILING_STOP')),
    CONSTRAINT CHK_orders_side       CHECK (side          IN ('BUY','SELL')),
    CONSTRAINT CHK_orders_status     CHECK (status        IN ('NEW','PARTIALLY_FILLED','FILLED','CANCELLED','REJECTED','EXPIRED','PENDING_CANCEL')),
    CONSTRAINT CHK_orders_tif        CHECK (time_in_force IN ('GTC','IOC','FOK','DAY','GTD'))
);

-- -----------------------------------------------------------------------------
-- Table: signal
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS signal (
    signal_id      INTEGER PRIMARY KEY AUTOINCREMENT,
    strategy_id    INTEGER NOT NULL,
    symbol_id      INTEGER NOT NULL,
    signal_time    INTEGER NOT NULL,
    signal_type    TEXT    NOT NULL,
    direction      TEXT    NOT NULL,
    strength       REAL    NOT NULL DEFAULT 1.0,
    price_target   REAL    NULL,
    stop_loss      REAL    NULL,
    take_profit    REAL    NULL,
    timeframe      TEXT    NULL,
    indicator_json TEXT    NULL,
    order_id       INTEGER NULL,
    status         TEXT    NOT NULL DEFAULT 'PENDING',
    expiry_time    INTEGER NULL,
    notes          TEXT    NULL,
    created_at     TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%SZ','now')),
    CONSTRAINT FK_signal_strategy   FOREIGN KEY (strategy_id) REFERENCES strategy(strategy_id) ON DELETE CASCADE,
    CONSTRAINT FK_signal_symbol     FOREIGN KEY (symbol_id)   REFERENCES symbols(symbol_id)   ON DELETE CASCADE,
    CONSTRAINT FK_signal_order      FOREIGN KEY (order_id)    REFERENCES orders(order_id),
    CONSTRAINT CHK_signal_type      CHECK (signal_type IN ('ENTRY','EXIT','MODIFY','ALERT')),
    CONSTRAINT CHK_signal_direction CHECK (direction   IN ('LONG','SHORT','FLAT')),
    CONSTRAINT CHK_signal_status    CHECK (status      IN ('PENDING','EXECUTED','EXPIRED','REJECTED','CANCELLED'))
);

-- -----------------------------------------------------------------------------
-- Table: positions
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS positions (
    position_id     INTEGER PRIMARY KEY AUTOINCREMENT,
    symbol_id       INTEGER NOT NULL,
    strategy_id     INTEGER NOT NULL,
    direction       TEXT    NOT NULL,
    quantity        REAL    NOT NULL,
    avg_entry_price REAL    NOT NULL,
    current_price   REAL    NULL,
    unrealized_pnl  REAL    NULL,
    realized_pnl    REAL    NOT NULL DEFAULT 0.0,
    opened_at       INTEGER NOT NULL,
    updated_at      TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%SZ','now')),
    CONSTRAINT FK_positions_symbol   FOREIGN KEY (symbol_id)   REFERENCES symbols(symbol_id)   ON DELETE CASCADE,
    CONSTRAINT FK_positions_strategy FOREIGN KEY (strategy_id) REFERENCES strategy(strategy_id) ON DELETE CASCADE,
    CONSTRAINT UQ_positions_unique   UNIQUE (symbol_id, strategy_id, direction),
    CONSTRAINT CHK_positions_dir     CHECK (direction IN ('LONG','SHORT')),
    CONSTRAINT CHK_positions_qty     CHECK (quantity >= 0)
);

-- -----------------------------------------------------------------------------
-- Table: performance_metrics
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS performance_metrics (
    metric_id    INTEGER PRIMARY KEY AUTOINCREMENT,
    strategy_id  INTEGER NOT NULL,
    date         TEXT    NOT NULL,
    total_pnl    REAL    NOT NULL,
    win_rate     REAL    NULL,
    sharpe_ratio REAL    NULL,
    max_drawdown REAL    NULL,
    trade_count  INTEGER NOT NULL DEFAULT 0,
    created_at   TEXT    NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%SZ','now')),
    CONSTRAINT FK_perf_strategy      FOREIGN KEY (strategy_id) REFERENCES strategy(strategy_id) ON DELETE CASCADE,
    CONSTRAINT UQ_perf_strategy_date UNIQUE (strategy_id, date)
);

-- -----------------------------------------------------------------------------
-- Table: trades  (individual tick trades from the message queue)
-- Columns match INSERT in Redissubscriber.cpp::storeTrade()
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS trades (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    symbol     TEXT    NOT NULL,
    price      REAL    NOT NULL,
    quantity   REAL    NOT NULL,
    timestamp  INTEGER NOT NULL,
    trade_id   TEXT    NULL,
    side       TEXT    NULL,
    created_at INTEGER NOT NULL DEFAULT (strftime('%s','now') * 1000)
);

-- -----------------------------------------------------------------------------
-- Table: ticker_updates  (best-bid/ask snapshots from the message queue)
-- Columns match INSERT in Redissubscriber.cpp::storeTicker()
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS ticker_updates (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    symbol     TEXT    NOT NULL,
    bid        REAL    NULL,
    ask        REAL    NULL,
    last       REAL    NULL,
    volume     REAL    NULL,
    timestamp  INTEGER NOT NULL,
    created_at INTEGER NOT NULL DEFAULT (strftime('%s','now') * 1000)
);

-- -----------------------------------------------------------------------------
-- Table: raw_messages  (audit log of every inbound MQ message)
-- Columns match INSERT in Redissubscriber.cpp::storeRawMessage()
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS raw_messages (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    channel     TEXT    NOT NULL,
    message     TEXT    NOT NULL,
    received_at INTEGER NOT NULL,
    processed   INTEGER NOT NULL DEFAULT 0
);

-- =============================================================================
-- INDEXES
-- =============================================================================

CREATE INDEX IF NOT EXISTS idx_symbols_ticker      ON symbols(ticker);
CREATE INDEX IF NOT EXISTS idx_symbols_exchange    ON symbols(exchange);
CREATE INDEX IF NOT EXISTS idx_symbols_asset_class ON symbols(asset_class);
CREATE INDEX IF NOT EXISTS idx_symbols_is_active   ON symbols(is_active);

CREATE INDEX IF NOT EXISTS idx_ohlcv_symbol_time      ON ohlcv(symbol_id, open_time DESC);
CREATE INDEX IF NOT EXISTS idx_ohlcv_symbol_timeframe ON ohlcv(symbol_id, timeframe, open_time DESC);
CREATE INDEX IF NOT EXISTS idx_ohlcv_open_time        ON ohlcv(open_time);
CREATE INDEX IF NOT EXISTS idx_ohlcv_is_closed        ON ohlcv(is_closed);

CREATE INDEX IF NOT EXISTS idx_ob_symbol_time ON orderbook(symbol_id, snapshot_time DESC);
CREATE INDEX IF NOT EXISTS idx_ob_side_price  ON orderbook(symbol_id, side, price);
CREATE INDEX IF NOT EXISTS idx_ob_level       ON orderbook(symbol_id, level);

CREATE INDEX IF NOT EXISTS idx_obu_symbol    ON orderbook_updates(symbol);
CREATE INDEX IF NOT EXISTS idx_obu_timestamp ON orderbook_updates(timestamp DESC);

CREATE INDEX IF NOT EXISTS idx_strategy_status ON strategy(status);
CREATE INDEX IF NOT EXISTS idx_strategy_mode   ON strategy(mode);
CREATE INDEX IF NOT EXISTS idx_strategy_name   ON strategy(name);

CREATE UNIQUE INDEX IF NOT EXISTS idx_orders_client_id ON orders(client_order_id);
CREATE INDEX IF NOT EXISTS idx_orders_exchange_id      ON orders(exchange_order_id);
CREATE INDEX IF NOT EXISTS idx_orders_symbol_status    ON orders(symbol_id, status);
CREATE INDEX IF NOT EXISTS idx_orders_strategy         ON orders(strategy_id);
CREATE INDEX IF NOT EXISTS idx_orders_submitted_at     ON orders(submitted_at DESC);
CREATE INDEX IF NOT EXISTS idx_orders_status           ON orders(status);
CREATE INDEX IF NOT EXISTS idx_orders_time_range       ON orders(submitted_at, filled_at);

CREATE INDEX IF NOT EXISTS idx_signal_strategy      ON signal(strategy_id);
CREATE INDEX IF NOT EXISTS idx_signal_symbol        ON signal(symbol_id);
CREATE INDEX IF NOT EXISTS idx_signal_time          ON signal(signal_time DESC);
CREATE INDEX IF NOT EXISTS idx_signal_status        ON signal(status);
CREATE INDEX IF NOT EXISTS idx_signal_strategy_time ON signal(strategy_id, signal_time DESC);

CREATE INDEX IF NOT EXISTS idx_positions_strategy ON positions(strategy_id);
CREATE INDEX IF NOT EXISTS idx_positions_symbol   ON positions(symbol_id);

CREATE INDEX IF NOT EXISTS idx_trades_symbol    ON trades(symbol);
CREATE INDEX IF NOT EXISTS idx_trades_timestamp ON trades(timestamp DESC);

CREATE INDEX IF NOT EXISTS idx_ticker_symbol    ON ticker_updates(symbol);
CREATE INDEX IF NOT EXISTS idx_ticker_timestamp ON ticker_updates(timestamp DESC);

CREATE INDEX IF NOT EXISTS idx_raw_channel   ON raw_messages(channel);
CREATE INDEX IF NOT EXISTS idx_raw_processed ON raw_messages(processed);

-- =============================================================================
-- TRIGGERS  (SQLite syntax â€” no GO, no INSERTED pseudo-table)
-- =============================================================================

CREATE TRIGGER IF NOT EXISTS trg_symbols_updated_at
AFTER UPDATE ON symbols
BEGIN
    UPDATE symbols SET updated_at = strftime('%Y-%m-%dT%H:%M:%SZ','now')
    WHERE symbol_id = NEW.symbol_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_strategy_updated_at
AFTER UPDATE ON strategy
BEGIN
    UPDATE strategy SET updated_at = strftime('%Y-%m-%dT%H:%M:%SZ','now')
    WHERE strategy_id = NEW.strategy_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_orders_updated_at
AFTER UPDATE ON orders
BEGIN
    UPDATE orders SET updated_at = strftime('%Y-%m-%dT%H:%M:%SZ','now')
    WHERE order_id = NEW.order_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_positions_updated_at
AFTER UPDATE ON positions
BEGIN
    UPDATE positions SET updated_at = strftime('%Y-%m-%dT%H:%M:%SZ','now')
    WHERE position_id = NEW.position_id;
END;

-- =============================================================================
-- VIEWS
-- =============================================================================

CREATE VIEW IF NOT EXISTS vw_active_positions AS
SELECT
    p.position_id,
    s.ticker,
    s.exchange,
    p.direction,
    p.quantity,
    p.avg_entry_price,
    p.current_price,
    CASE
        WHEN p.direction = 'LONG' THEN (p.current_price - p.avg_entry_price) * p.quantity
        ELSE (p.avg_entry_price - p.current_price) * p.quantity
    END AS unrealized_pnl,
    str.name AS strategy_name,
    p.opened_at,
    p.updated_at
FROM positions p
INNER JOIN symbols  s   ON p.symbol_id   = s.symbol_id
INNER JOIN strategy str ON p.strategy_id = str.strategy_id
WHERE p.quantity > 0;

CREATE VIEW IF NOT EXISTS vw_recent_signals AS
SELECT
    sig.signal_id,
    str.name AS strategy_name,
    s.ticker,
    sig.signal_time,
    sig.signal_type,
    sig.direction,
    sig.strength,
    sig.price_target,
    sig.stop_loss,
    sig.status,
    o.client_order_id,
    sig.created_at
FROM signal sig
INNER JOIN strategy str ON sig.strategy_id = str.strategy_id
INNER JOIN symbols  s   ON sig.symbol_id   = s.symbol_id
LEFT  JOIN orders   o   ON sig.order_id    = o.order_id;

