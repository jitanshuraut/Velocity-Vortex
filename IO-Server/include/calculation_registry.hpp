#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "calculation_handler.hpp"

// ─── CalculationRegistry ──────────────────────────────────────────────────────
//
// Central catalog of all registered ICalculationHandler implementations.
// Decouples the HTTP router from concrete calculation types so that adding
// a new method requires zero changes to routing or startup code — just:
//
//   registry->register_handler(std::make_shared<MySMAHandler>());
//
class CalculationRegistry
{
public:
    using HandlerPtr = std::shared_ptr<ICalculationHandler>;

    // Register a handler. Uses handler->name() as the lookup key.
    // Overwrites any previously registered handler with the same name.
    void register_handler(HandlerPtr handler);

    // Returns the handler for `name`, or nullptr if not found.
    HandlerPtr get(const std::string &name) const;

    bool has(const std::string &name) const;

    // Returns all registered method names (unordered).
    std::vector<std::string> list() const;

    // Builds a JSON object {"methods": [{name, description}, ...]}
    crow::json::wvalue list_json() const;

private:
    std::unordered_map<std::string, HandlerPtr> handlers_;
};
