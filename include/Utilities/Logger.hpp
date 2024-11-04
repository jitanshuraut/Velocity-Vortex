#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "IO-Broker-Core/RedisDBManager.hpp"
class Logger
{
public:
    Logger() = default;
    static void initialize();
    static std::shared_ptr<RedisDBManager> &getInstance();

private:
    static std::shared_ptr<RedisDBManager> logger;
};

#endif
