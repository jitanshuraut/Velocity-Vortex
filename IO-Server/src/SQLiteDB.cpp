#include "SQLiteDB.hpp"
#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

// ============================================================================
// PIMPL Implementation
// ============================================================================
struct SQLiteDB::Impl
{
    sqlite3 *db = nullptr;
    std::string path;

    explicit Impl(const std::string &p) : path(p) {}
    ~Impl()
    {
        if (db)
            sqlite3_close(db);
    }
};

// ============================================================================
// BindValue implementations
// ============================================================================
BindValue BindValue::from_int(int64_t v)
{
    BindValue b;
    b.type = Type::INT;
    b.ival = v;
    return b;
}

BindValue BindValue::from_double(double v)
{
    BindValue b;
    b.type = Type::DOUBLE;
    b.dval = v;
    return b;
}

BindValue BindValue::from_text(const std::string &v)
{
    BindValue b;
    b.type = Type::TEXT;
    b.sval = v;
    return b;
}

BindValue BindValue::null()
{
    BindValue b;
    b.type = Type::NUL;
    return b;
}

// ============================================================================
// SQLiteDB implementation
// ============================================================================
SQLiteDB::SQLiteDB(const std::string &path) : pimpl(std::make_unique<Impl>(path))
{
    int rc = sqlite3_open(path.c_str(), &pimpl->db);
    if (rc != SQLITE_OK)
    {
        std::string err = sqlite3_errmsg(pimpl->db);
        sqlite3_close(pimpl->db);
        pimpl->db = nullptr;
        throw std::runtime_error("[SQLiteDB] Failed to open database: " + err);
    }

    // Set pragmas for performance
    exec("PRAGMA journal_mode=WAL;");
    exec("PRAGMA synchronous=NORMAL;");
    exec("PRAGMA foreign_keys=ON;");
    exec("PRAGMA cache_size=-65536;"); // 64MB
    exec("PRAGMA temp_store=MEMORY;");
    exec("PRAGMA mmap_size=268435456;"); // 256MB
}

SQLiteDB::~SQLiteDB() = default;

SQLiteDB::SQLiteDB(SQLiteDB &&o) noexcept = default;
SQLiteDB &SQLiteDB::operator=(SQLiteDB &&o) noexcept = default;

// ============================================================================
// Core Database Operations
// ============================================================================
void SQLiteDB::exec(const std::string &sql,
                    const std::vector<BindValue> &params,
                    const std::string &ctx)
{
    sqlite3_stmt *stmt = nullptr;

    // Prepare statement
    if (sqlite3_prepare_v2(pimpl->db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("[SQLiteDB::prepare] " + ctx + ": " +
                                 sqlite3_errmsg(pimpl->db));
    }

    // Bind parameters
    for (int i = 0; i < static_cast<int>(params.size()); ++i)
    {
        const auto &p = params[i];
        int idx = i + 1;
        int rc = SQLITE_OK;

        switch (p.type)
        {
        case BindValue::Type::INT:
            rc = sqlite3_bind_int64(stmt, idx, p.ival);
            break;
        case BindValue::Type::DOUBLE:
            rc = sqlite3_bind_double(stmt, idx, p.dval);
            break;
        case BindValue::Type::TEXT:
            rc = sqlite3_bind_text(stmt, idx, p.sval.c_str(),
                                   -1, SQLITE_TRANSIENT);
            break;
        case BindValue::Type::NUL:
            rc = sqlite3_bind_null(stmt, idx);
            break;
        }

        if (rc != SQLITE_OK)
        {
            sqlite3_finalize(stmt);
            throw std::runtime_error("[SQLiteDB::bind] " + ctx + ": " +
                                     sqlite3_errmsg(pimpl->db));
        }
    }

    // Execute
    int rc;
    do
    {
        rc = sqlite3_step(stmt);
    } while (rc == SQLITE_ROW);

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE && rc != SQLITE_OK)
    {
        throw std::runtime_error("[SQLiteDB::exec] " + ctx + ": " +
                                 sqlite3_errmsg(pimpl->db));
    }
}

RowSet SQLiteDB::query(const std::string &sql,
                       const std::vector<BindValue> &params,
                       const std::string &ctx)
{
    sqlite3_stmt *stmt = nullptr;

    // Prepare statement
    if (sqlite3_prepare_v2(pimpl->db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("[SQLiteDB::prepare] " + ctx + ": " +
                                 sqlite3_errmsg(pimpl->db));
    }

    // Bind parameters
    for (int i = 0; i < static_cast<int>(params.size()); ++i)
    {
        const auto &p = params[i];
        int idx = i + 1;
        int rc = SQLITE_OK;

        switch (p.type)
        {
        case BindValue::Type::INT:
            rc = sqlite3_bind_int64(stmt, idx, p.ival);
            break;
        case BindValue::Type::DOUBLE:
            rc = sqlite3_bind_double(stmt, idx, p.dval);
            break;
        case BindValue::Type::TEXT:
            rc = sqlite3_bind_text(stmt, idx, p.sval.c_str(),
                                   -1, SQLITE_TRANSIENT);
            break;
        case BindValue::Type::NUL:
            rc = sqlite3_bind_null(stmt, idx);
            break;
        }

        if (rc != SQLITE_OK)
        {
            sqlite3_finalize(stmt);
            throw std::runtime_error("[SQLiteDB::bind] " + ctx + ": " +
                                     sqlite3_errmsg(pimpl->db));
        }
    }

    // Fetch results
    RowSet rows;
    int rc;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        Row row;
        int ncols = sqlite3_column_count(stmt);
        for (int i = 0; i < ncols; ++i)
        {
            const char *name = sqlite3_column_name(stmt, i);
            const char *val = reinterpret_cast<const char *>(
                sqlite3_column_text(stmt, i));
            row[name ? name : ""] = val ? val : "";
        }
        rows.push_back(std::move(row));
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        throw std::runtime_error("[SQLiteDB::query] " + ctx + ": " +
                                 sqlite3_errmsg(pimpl->db));
    }

    return rows;
}

// Scalar queries
int64_t SQLiteDB::scalar_int(const std::string &sql,
                             const std::vector<BindValue> &params,
                             int64_t default_val,
                             const std::string &ctx)
{
    auto rows = query(sql, params, ctx);
    if (rows.empty() || rows[0].empty())
        return default_val;
    try
    {
        return std::stoll(rows[0].begin()->second);
    }
    catch (...)
    {
        return default_val;
    }
}

double SQLiteDB::scalar_double(const std::string &sql,
                               const std::vector<BindValue> &params,
                               double default_val,
                               const std::string &ctx)
{
    auto rows = query(sql, params, ctx);
    if (rows.empty() || rows[0].empty())
        return default_val;
    try
    {
        return std::stod(rows[0].begin()->second);
    }
    catch (...)
    {
        return default_val;
    }
}

std::string SQLiteDB::scalar_text(const std::string &sql,
                                  const std::vector<BindValue> &params,
                                  const std::string &default_val,
                                  const std::string &ctx)
{
    auto rows = query(sql, params, ctx);
    if (rows.empty() || rows[0].empty())
        return default_val;
    return rows[0].begin()->second;
}

// ============================================================================
// File Operations
// ============================================================================
std::string SQLiteDB::read_file(const std::string &filepath) const
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        throw std::runtime_error("[SQLiteDB] Cannot open file: " + filepath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void SQLiteDB::execute_script(const std::string &sql_script, const std::string &ctx)
{
    // sqlite3_exec handles the full SQL grammar including BEGIN...END trigger
    // blocks that contain their own semicolons — far safer than manual splitting.
    char *errmsg = nullptr;
    int rc = sqlite3_exec(pimpl->db, sql_script.c_str(), nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK)
    {
        std::string err = errmsg ? errmsg : "unknown sqlite3 error";
        sqlite3_free(errmsg);
        // Log the failure but don't throw — bootstrap continues with remaining files.
        std::cerr << "[SQLiteDB] Script error in '" << ctx << "': " << err << "\n";
    }
}

void SQLiteDB::bootstrap_from_file(const std::string &filepath, const std::string &ctx)
{
    std::cout << "[SQLiteDB] Loading schema from: " << filepath << "\n";
    std::string sql = read_file(filepath);
    execute_script(sql, ctx.empty() ? filepath : ctx);
    std::cout << "[SQLiteDB] Successfully loaded: " << filepath << "\n";
}

void SQLiteDB::bootstrap_from_directory(const std::string &directory_path)
{
    if (!fs::exists(directory_path))
    {
        throw std::runtime_error("[SQLiteDB] Directory does not exist: " + directory_path);
    }

    std::cout << "\n[SQLiteDB] ===== Bootstrap Starting =====\n";
    std::cout << "[SQLiteDB] Directory: " << directory_path << "\n";

    // Collect all .sql files
    std::vector<fs::path> sql_files;
    for (const auto &entry : fs::directory_iterator(directory_path))
    {
        if (entry.path().extension() == ".sql")
        {
            sql_files.push_back(entry.path());
        }
    }

    // Sort files to ensure consistent order
    std::sort(sql_files.begin(), sql_files.end());

    // Execute each file
    for (const auto &file : sql_files)
    {
        try
        {
            bootstrap_from_file(file.string(), file.filename().string());
        }
        catch (const std::exception &e)
        {
            std::cerr << "[ERROR] Failed to load " << file << ": " << e.what() << "\n";
            throw; // Re-throw - bootstrap should be atomic
        }
    }

    std::cout << "[SQLiteDB] ===== Bootstrap Complete =====\n\n";
}

// ============================================================================
// Utility
// ============================================================================
int64_t SQLiteDB::last_insert_rowid() const
{
    return sqlite3_last_insert_rowid(pimpl->db);
}

int SQLiteDB::changes() const
{
    return sqlite3_changes(pimpl->db);
}