#include "calculation_registry.hpp"

void CalculationRegistry::register_handler(HandlerPtr handler)
{
    handlers_[handler->name()] = std::move(handler);
}

CalculationRegistry::HandlerPtr CalculationRegistry::get(const std::string &name) const
{
    auto it = handlers_.find(name);
    return it != handlers_.end() ? it->second : nullptr;
}

bool CalculationRegistry::has(const std::string &name) const
{
    return handlers_.count(name) > 0;
}

std::vector<std::string> CalculationRegistry::list() const
{
    std::vector<std::string> names;
    names.reserve(handlers_.size());
    for (const auto &[k, _] : handlers_)
        names.push_back(k);
    return names;
}

crow::json::wvalue CalculationRegistry::list_json() const
{
    std::vector<crow::json::wvalue> methods;
    for (const auto &[name, handler] : handlers_)
    {
        crow::json::wvalue m;
        m["name"] = handler->name();
        m["description"] = handler->description();
        methods.push_back(std::move(m));
    }
    crow::json::wvalue result;
    result["methods"] = std::move(methods);
    return result;
}
