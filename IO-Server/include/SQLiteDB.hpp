#ifndef SQLITEDB_HPP
#define SQLITEDB_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <sqlite3.h>

// Row types for query results
using Row = std::map<std::string, std::string>;
using RowSet = std::vector<Row>;

// Type-safe parameter binding
struct BindValue
{
    enum class Type
    {
        INT,
        DOUBLE,
        TEXT,
        NUL
    };
    Type type = Type::NUL;
    int64_t ival = 0;
    double dval = 0.0;
    std::string sval;

    static BindValue from_int(int64_t v);
    static BindValue from_double(double v);
    static BindValue from_text(const std::string &v);
    static BindValue null();
};

// Convenience shorthand helpers
inline BindValue bind_int(int64_t v) { return BindValue::from_int(v); }
inline BindValue bind_real(double v) { return BindValue::from_double(v); }
inline BindValue bind_text(const std::string &v) { return BindValue::from_text(v); }
inline BindValue bind_null() { return BindValue::null(); }

// Main database class
class SQLiteDB
{
public:
    // Constructor/Destructor
    explicit SQLiteDB(const std::string &path);
    ~SQLiteDB();

    // Non-copyable, movable
    SQLiteDB(const SQLiteDB &) = delete;
    SQLiteDB &operator=(const SQLiteDB &) = delete;
    SQLiteDB(SQLiteDB &&o) noexcept;
    SQLiteDB &operator=(SQLiteDB &&o) noexcept;

    // ── Core Database Operations ───────────────────────────────────────────
    void exec(const std::string &sql,
              const std::vector<BindValue> &params = {},
              const std::string &ctx = "");

    RowSet query(const std::string &sql,
                 const std::vector<BindValue> &params = {},
                 const std::string &ctx = "");

    int64_t scalar_int(const std::string &sql,
                       const std::vector<BindValue> &params = {},
                       int64_t default_val = 0,
                       const std::string &ctx = "");

    double scalar_double(const std::string &sql,
                         const std::vector<BindValue> &params = {},
                         double default_val = 0.0,
                         const std::string &ctx = "");

    std::string scalar_text(const std::string &sql,
                            const std::vector<BindValue> &params = {},
                            const std::string &default_val = "",
                            const std::string &ctx = "");

    // ── Bootstrap - Read and execute SQL files ────────────────────────────
    void bootstrap_from_file(const std::string &filepath, const std::string &ctx = "");
    void bootstrap_from_directory(const std::string &directory_path);

    // ── Utility ────────────────────────────────────────────────────────────
    sqlite3 *handle() const; // moved out-of-line to avoid accessing incomplete Impl
    int64_t last_insert_rowid() const;
    int changes() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

    // Helper to execute multiple SQL statements from a string
    void execute_script(const std::string &sql_script, const std::string &ctx);

    // Helper to read file contents
    std::string read_file(const std::string &filepath) const;
};

#endif // SQLITEDB_HPP